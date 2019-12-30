#include "w_system_pch.h"
#include "w_compress_lzma.h"

using namespace wolf::system;

int w_compress_lzma::compress(const uint8_t* pSourceBuffer, w_compress_result* pCompressInfo)
{
	// set up properties
	CLzmaEncProps _props;
	LzmaEncProps_Init(&_props);

	if (pCompressInfo->size_in >= (1 << 20))
	{
		_props.dictSize = 1 << 20; //1mb dictionary
	}
	else
	{
		_props.dictSize = pCompressInfo->size_in; //smaller dictionary = faster!
	}
	_props.fb = 40;

	// prepare space for the encoded properties
	SizeT _props_size = 5;
	unsigned char _props_encoded[5];

	// allocate some space for the compression output
	// this is way more than necessary in most cases...
	// but better safe than sorry
	//   (a smarter implementation would use a growing buffer,
	//    but this requires a bunch of fuckery that is out of
	///   scope for this simple example)
	SizeT _output_size_64 = pCompressInfo->size_in * 2.0f;
	if (_output_size_64 < 1024)
	{
		_output_size_64 = 1024;
	}

	auto _compress_mem = (uint8_t*)malloc(_output_size_64 * sizeof(uint8_t));
	int _lzma_status = LzmaEncode(
		_compress_mem,
		&_output_size_64,
		pSourceBuffer,
		pCompressInfo->size_in,
		&_props,
		_props_encoded,
		&_props_size,
		0,
		NULL,
		&_alloc_funcs,
		&_alloc_funcs);

	pCompressInfo->size_out = _output_size_64 + 13;
	if (_lzma_status == SZ_OK)
	{
		// tricky: we have to generate the LZMA header
		// 5 bytes properties + 8 byte uncompressed size
		pCompressInfo->data = (char*)malloc((_output_size_64 + 13) * sizeof(char));
		
		memcpy(pCompressInfo->data, _props_encoded, 5);
		for (int i = 0; i < 8; i++)
		{
			pCompressInfo->data[5 + i] = (pCompressInfo->size_in >> (i * 8)) & 0xFF;
		}
		memcpy(pCompressInfo->data + 13, _compress_mem, _output_size_64);

		return 0;
	}
	return 1;
}

int w_compress_lzma::decompress(const uint8_t* pCompressedBuffer, w_compress_result* pDeCompressInfo)
{
	pDeCompressInfo->size_out = 0;
	if (pDeCompressInfo->size_in < 13)
	{
		return 1; // invalid header!
	}

	// extract the size from the header
	UInt64 _size_from_header = 0;
	for (int i = 0; i < 8; i++)
	{
		_size_from_header |= (pCompressedBuffer[5 + i] << (i * 8));
	}

	if (_size_from_header <= (256 * 1024 * 1024))
	{
		pDeCompressInfo->data = (char*)malloc(_size_from_header);

		ELzmaStatus _lzma_status;
		SizeT _proc_out_size = _size_from_header, _proc_in_size = pDeCompressInfo->size_in - 13;
		int status = LzmaDecode(
			(uint8_t*)pDeCompressInfo->data,
			&_proc_out_size,
			&pCompressedBuffer[13],
			&_proc_in_size,
			pCompressedBuffer,
			5,
			LZMA_FINISH_END,
			&_lzma_status,
			&_alloc_funcs);

		if (status == SZ_OK && _proc_out_size == _size_from_header)
		{
			pDeCompressInfo->data[_proc_out_size] = '\0';
			pDeCompressInfo->size_out = _proc_out_size;
			return 0;
		}
	}
	return 1;
}