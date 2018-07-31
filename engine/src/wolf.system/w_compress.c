#include "w_compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lz4/lz4.h"

#define IN_CHUNK_SIZE  (16 * 1024)

int compress_buffer_c(
	/*_In_*/	char* pSrcBuffer,
	/*_In_*/	w_compress_mode pMode,
	/*_In_*/	int pAcceleration,
	/*_Inout_*/	w_compress_result* pCompressInfo,
	/*_Inout_*/ char* pErrorLog)
{
	if (!pCompressInfo || !pSrcBuffer || pCompressInfo->size_in == 0)
	{
		return 1;
	}
		
	const int _max_dst_size = LZ4_compressBound(pCompressInfo->size_in);
	//allocate size for compressed data
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
	if (pMode == W_COMPRESS_DEFAULT)
	{
		_compressed_buffer_size = LZ4_compress_default(pSrcBuffer, pCompressInfo->data, pCompressInfo->size_in, _max_dst_size);
	}
	else
	{
		_compressed_buffer_size = LZ4_compress_fast_force(pSrcBuffer, pCompressInfo->data, pCompressInfo->size_in, _max_dst_size, pAcceleration);
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
				"could not reallocate memory of compressed buffer. trace_info: w_compress::compress_buffer_c");
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