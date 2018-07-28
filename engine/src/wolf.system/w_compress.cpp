#include "w_system_pch.h"
#include "w_compress.h"

#define IN_CHUNK_SIZE  (16 * 1024)

using namespace wolf;
using namespace wolf::system;

typedef struct
{
	int error;
	unsigned long long size_in;
	unsigned long long size_out;
} compressResult_t;

static const LZ4F_preferences_t sPrefs =
{
	{ LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame, 0 /* unknown content size */, 0 /* no dictID */ , LZ4F_noBlockChecksum },
	0,   /* compression level; 0 == default */
	0,   /* autoflush */
	0,   /* favor decompression speed */
	{ 0, 0, 0 },  /* reserved, must be set to 0 */
};

// performs fwrite(), ensure operation success
static W_RESULT safe_file_write(_In_ void* pBuffer, _In_ size_t pEltSize, size_t pNbElt, FILE* pFile)
{
	size_t const _written_size = fwrite(pBuffer, pEltSize, pNbElt, pFile);
	size_t const _expected_size = pEltSize * pNbElt;//TODO: should check for overflow
	if (_written_size < _expected_size)
	{
		if (ferror(pFile))
		{
			logger.error("w_compress::safe_write failed");
		}
		else
		{
			logger.error("w_compress::safe_write short write");
		}
		return W_RESULT::W_FAILED;
	}
	return W_RESULT::W_PASSED;
}

static w_compress_result _compress_file_internal(
	FILE* pFileIn,
	FILE* pFileOut,
	LZ4F_compressionContext_t pContext,
	void* pInBuffer,
	size_t pInChunkSize,
	void* pOutBuffer,
	size_t pOutCapacity)
{
	const char* _trace_info = "compress_file_internal";

	w_compress_result _result = { 1, 0, 0 };  // error by default

	if (pOutCapacity >= LZ4F_HEADER_SIZE_MAX)
	{
		logger.error("output capacity is greater than LZ4F_HEADER_SIZE_MAX");
		return _result;
	}
	if (pOutCapacity >= LZ4F_compressBound(pInChunkSize, &sPrefs))
	{
		logger.error("output capacity is greater than LZ4F_compressBound");
		return _result;
	}

	unsigned long long _count_in = 0, _count_out;

	//write frame header
	{
		size_t const _header_size = LZ4F_compressBegin(pContext, pOutBuffer, pOutCapacity, &sPrefs);
		if (LZ4F_isError(_header_size))
		{
			logger.write("failed to start compression: error {}. trace info: {}", _header_size, _trace_info);
			return _result;
		}
		_count_out = _header_size;
		logger.error("buffer size is {} bytes, header size {} bytes", pOutCapacity, _header_size);
		safe_file_write(pOutBuffer, 1, _header_size, pFileOut);
	}

	//stream file
	for (;;)
	{
		size_t const _read_size = fread(pInBuffer, 1, IN_CHUNK_SIZE, pFileIn);
		if (_read_size == 0) break; //nothing left to read from input file
		_count_in += _read_size;

		size_t const _compressed_size = LZ4F_compressUpdate(
			pContext,
			pOutBuffer,
			pOutCapacity,
			pInBuffer,
			_read_size,
			NULL);
		if (LZ4F_isError(_compressed_size))
		{
			logger.error("compression failed: error {}", _compressed_size);
			return _result;
		}

		//logger.error("Writing {} bytes", _compressed_size);
		safe_file_write(pOutBuffer, 1, _compressed_size, pFileOut);
		_count_out += _compressed_size;
	}

	//flush whatever remains within internal buffers */
	{
		size_t const _compressed_size = LZ4F_compressEnd(
			pContext,
			pOutBuffer,
			pOutCapacity,
			NULL);
		if (LZ4F_isError(_compressed_size))
		{
			logger.error("failed to end compression: error {}", _compressed_size);
			return _result;
		}

		//printf("Writing %zu bytes\n", compressedSize);
		safe_file_write(pOutBuffer, 1, _compressed_size, pFileOut);
		_count_out += _compressed_size;
	}

	_result.size_in = _count_in;
	_result.size_out = _count_out;
	_result.error = 0;

	return _result;
}

w_compress_result w_compress::compress_file(_In_ FILE* pFileStreamIn, _Inout_ FILE* pCompressedFileOut)
{
	w_compress_result _result = { 1, 0, 0 };//error by default

	if (!pFileStreamIn || !pCompressedFileOut) return _result;

	//ressource allocation
	LZ4F_compressionContext_t _lz4_ctx;
	size_t const _ctx_creation = LZ4F_createCompressionContext(&_lz4_ctx, LZ4F_VERSION);

	void* const _src = malloc(IN_CHUNK_SIZE);
	size_t const _out_buffer_capacity = LZ4F_compressBound(IN_CHUNK_SIZE, &sPrefs);   /* large enough for any input <= IN_CHUNK_SIZE */
	void* const _out_buffer = malloc(_out_buffer_capacity);

	if (!LZ4F_isError(_ctx_creation) && _src && _out_buffer)
	{
		_result = _compress_file_internal(
			pFileStreamIn,
			pCompressedFileOut,
			_lz4_ctx,
			_src,
			IN_CHUNK_SIZE,
			_out_buffer,
			_out_buffer_capacity);
	}
	else
	{
		logger.error("ressource allocation failed. trace_info: w_compress::compress_file");
	}

	LZ4F_freeCompressionContext(_lz4_ctx);//supports free on NULL
	free(_src);
	free(_out_buffer);
	return _result;
}

w_compress_result w_compress::decompress_file(_In_ FILE* pCompressedFileIn, _Inout_ FILE* pFileStreamOut)
{
	w_compress_result _result = { 1, 0, 0 };//error by default

	if (!pCompressedFileIn || !pFileStreamOut) return _result;


	return _result;
}
