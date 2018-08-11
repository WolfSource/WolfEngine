#include "w_compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lz4/lz4.h"

#define IN_CHUNK_SIZE  (16 * 1024)

int compress_buffer_c(
	/*_In_*/	const char* pSrcBuffer,
	/*_In_*/	w_compress_mode pMode,
	/*_In_*/	int pAcceleration,
	/*_Inout_*/	w_compress_result* pCompressInfo,
	/*_Inout_*/ char* pErrorLog)
{
	if (!pCompressInfo || !pSrcBuffer || pCompressInfo->size_in == 0) return 1;
	
	//allocate size for compressed data
	const int _max_dst_size = LZ4_compressBound(pCompressInfo->size_in);
	pCompressInfo->data = malloc(_max_dst_size);
	if (!pCompressInfo->data)
	{
		snprintf(
			pErrorLog,
			strlen(pErrorLog),
			"allocating memory for compressed buffer. trace_info: w_compress::compress_buffer_c");
		return 1;
	}

	int _compressed_buffer_size = -1;
	if (pMode == W_FAST)
	{
		_compressed_buffer_size = LZ4_compress_fast_force(pSrcBuffer, pCompressInfo->data, pCompressInfo->size_in, _max_dst_size, pAcceleration);
	}
	else
	{
		_compressed_buffer_size = LZ4_compress_default(pSrcBuffer, pCompressInfo->data, pCompressInfo->size_in, _max_dst_size);
	}

	if (_compressed_buffer_size > 0)
	{
		//realloc compress_data to free up memory
		pCompressInfo->size_out = (size_t)_compressed_buffer_size;
		pCompressInfo->data = (char*)realloc(pCompressInfo->data, pCompressInfo->size_out);
		if (!pCompressInfo->data)
		{
			snprintf(
				pErrorLog,
				strlen(pErrorLog),
				"could not fit memory of compressed buffer. trace_info: w_compress::compress_buffer_c");
			return 1;
		}
		return 0;
	}
	else
	{
		snprintf(
			pErrorLog,
			strlen(pErrorLog),
			"could not compress. trace_info: w_compress::compress_buffer_c");
		return 1;
	}
}

int decompress_buffer_c(
	/*_In_*/	const char* pCompressedBuffer,
	/*_Inout_*/	w_compress_result* pDecompressInfo,
	/*_Inout_*/ char* pErrorLog)
{
	if (!pCompressedBuffer || pDecompressInfo->size_in == 0) return 1;

	//allocate memory for decompress
	size_t _destination_capacity = pDecompressInfo->size_in * 2;
	pDecompressInfo->data = (char*)malloc(_destination_capacity);
	if (!pDecompressInfo->data)
	{
		snprintf(
			pErrorLog,
			strlen(pErrorLog),
			"error on allocate buffer for decompressed buffer. trace_info: w_compress::decompress_buffer_c");
		return 1;
	}

	int _decompressed_size = -1;
	int _number_of_try = 10;
	
	while (_number_of_try > 0)
	{
		_decompressed_size = LZ4_decompress_safe(
				pCompressedBuffer,
				&pDecompressInfo->data[0],
				(int)pDecompressInfo->size_in,
				_destination_capacity);
		if (_decompressed_size >= 0)
		{
			break;
		}
		else
		{
			_destination_capacity *= 2;
			pDecompressInfo->data = (char*)realloc(pDecompressInfo->data, _destination_capacity);
			if (!pDecompressInfo->data)
			{
				snprintf(
					pErrorLog,
					strlen(pErrorLog),
					"could not re-allocate memory of de-compressed buffer. trace_info: w_compress::compress_buffer_c");
				break;
			}
		}

		_number_of_try--;
	}

	if (_decompressed_size <= 0)
	{
		snprintf(
			pErrorLog,
			strlen(pErrorLog),
			"decompress size must be greater than zero. trace_info: w_compress::decompress_buffer_c");
		free(pDecompressInfo->data);
		pDecompressInfo->data = NULL;
		pDecompressInfo->size_out = 0;
		return 1;
	}
	else
	{
		pDecompressInfo->size_out = _decompressed_size;
		pDecompressInfo->data = (char*)realloc(pDecompressInfo->data, pDecompressInfo->size_out);
		if (!pDecompressInfo->data)
		{
			snprintf(
				pErrorLog,
				strlen(pErrorLog),
				"could not fit memory for de-compressed buffer. trace_info: w_compress::compress_buffer_c");
			return 1;
		}
	}
	return 0;
}
