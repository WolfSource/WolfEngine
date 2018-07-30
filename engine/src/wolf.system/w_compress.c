#include "w_compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lz4/lz4frame.h"

#define IN_CHUNK_SIZE  (16 * 1024)

static const LZ4F_preferences_t sPrefs = 
{
	{ LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame, 0 /* unknown content size */, 0 /* no dictID */ , LZ4F_noBlockChecksum },
	0,   //compression level; 0 == default 
	0,   //autoflush
	0,   //favor decompression speed
	{ 0, 0, 0 },  //reserved, must be set to 0
};

//safe_fwrite() : performs fwrite(), ensure operation success, or immediately exit()
static int safe_fwrite(void* pBuffer, size_t pEltSize, size_t pNbElt, FILE* pFile, _Inout_ char* pErrorLog)
{
	size_t const _written_size = fwrite(pBuffer, pEltSize, pNbElt, pFile);
	size_t const _expected_size = pEltSize * pNbElt;//note : should check for overflow
	if (_written_size < _expected_size)
	{
		if (ferror(pFile))
		{
			snprintf(pErrorLog, strlen(pErrorLog), "w_compress::safe_write failed");
		}
		else
		{
			snprintf(pErrorLog, strlen(pErrorLog), "w_compress::safe_write short write");
		}
		return 1;
	}
	return 0;
}

static w_compress_result compress_file_internal(
	FILE* pFileIn,
	FILE* pFileOut,
	LZ4F_compressionContext_t pContext,
	void* pInBuffer,
	size_t pInChunkSize,
	void* pOutBuffer,
	size_t pOutCapacity,
	_Inout_ char* pErrorLog)
{
	w_compress_result _result = { 1, 0, 0 };//error by default
	unsigned long long count_in = 0, count_out;

	if (pOutCapacity < LZ4F_HEADER_SIZE_MAX)
	{
		snprintf(pErrorLog, strlen(pErrorLog), "output capacity must be greater or equal to LZ4F_HEADER_SIZE_MAX. trace_info: w_compress::_compress_file_internal");
		return _result;
	}
	if (pOutCapacity < LZ4F_compressBound(pInChunkSize, &sPrefs))
	{
		snprintf(pErrorLog, strlen(pErrorLog), "output capacity be greater or equal to LZ4F_compressBound. trace_info: w_compress::_compress_file_internal");
		return _result;
	}

	// write frame header
	{
		size_t const _header_size = LZ4F_compressBegin(pContext, pOutBuffer, pOutCapacity, &sPrefs);
		if (LZ4F_isError(_header_size))
		{
			snprintf(pErrorLog,
				strlen(pErrorLog),
				"failed to start compression. header size is %zu. trace_info: w_compress::_compress_file_internal",
				_header_size);
			return _result;
		}
		count_out = _header_size;
		if (safe_fwrite(pOutBuffer, 1, _header_size, pFileOut, pErrorLog))
		{
			return _result;
		}
	}

	//stream file
	char _write_error = 0;
	for (;;)
	{
		size_t const _read_size = fread(pInBuffer, 1, IN_CHUNK_SIZE, pFileIn);
		if (_read_size == 0) break; //nothing left to read from input file
		count_in += _read_size;

		size_t const _compressed_size = LZ4F_compressUpdate(pContext,
			pOutBuffer,
			pOutCapacity,
			pInBuffer,
			_read_size,
			NULL);
		if (LZ4F_isError(_compressed_size)) 
		{
			snprintf(pErrorLog, 
				strlen(pErrorLog), 
				"compression failed. compress size is %zu. trace_info: w_compress::_compress_file_internal", 
				_compressed_size);
			return _result;
		}

		if (safe_fwrite(pOutBuffer, 1, _compressed_size, pFileOut, pErrorLog))
		{
			_write_error = 1;
			break;
		}
		count_out += _compressed_size;
	}

	if (_write_error)
	{
		return _result;
	}

	//flush whatever remains within internal buffers
	{
		size_t const _compressed_size = LZ4F_compressEnd(
			pContext,
			pOutBuffer,
			pOutCapacity,
			NULL);
		if (LZ4F_isError(_compressed_size))
		{
			snprintf(
				pErrorLog, 
				strlen(pErrorLog), 
				"failed to end compression. compress size is %zu. trace_info: w_compress::_compress_file_internal", 
				_compressed_size);
			return _result;
		}

		if (safe_fwrite(pOutBuffer, 1, _compressed_size, pFileOut, pErrorLog))
		{
			return _result;
		}
		count_out += _compressed_size;
	}

	_result.size_in = count_in;
	_result.size_out = count_out;
	_result.error = 0;
	return _result;
}

//static size_t get_block_size(const LZ4F_frameInfo_t* pInfo, _Inout_ char* pErrorLog)
//{
//	switch (pInfo->blockSizeID)
//	{
//	case LZ4F_default:
//	case LZ4F_max64KB:  return 1 << 16;
//	case LZ4F_max256KB: return 1 << 18;
//	case LZ4F_max1MB:   return 1 << 20;
//	case LZ4F_max4MB:   return 1 << 22;
//	default:
//		snprintf(pErrorLog, strlen(pErrorLog),
//			"Impossible with expected frame specification: %d. trace_info: w_compress::get_block_size",
//			pInfo->blockSizeID);
//		return -1;//return must be int
//	}
//}

w_compress_result compress_file_c(FILE* pFileIn, FILE* pFileOut, _Inout_ char* pErrorLog)
{
	w_compress_result _result = { 1, 0, 0 };//error by default

	if (!pFileIn || !pFileOut) return _result;

	//resource allocation
	LZ4F_compressionContext_t _ctx = NULL;
	size_t const _ctx_creation = LZ4F_createCompressionContext(&_ctx, LZ4F_VERSION);
	void* const _src = malloc(IN_CHUNK_SIZE);
	size_t const _out_buffer_capacity = LZ4F_compressBound(IN_CHUNK_SIZE, &sPrefs);//large enough for any input <= IN_CHUNK_SIZE
	void* const _out_buffer = malloc(_out_buffer_capacity);

	if (!LZ4F_isError(_ctx_creation) && _src && _out_buffer && _ctx)
	{
		_result = compress_file_internal(
			pFileIn,
			pFileOut,
			_ctx,
			_src,
			IN_CHUNK_SIZE,
			_out_buffer,
			_out_buffer_capacity,
			pErrorLog);
	}
	else
	{
		snprintf(pErrorLog, strlen(pErrorLog), "resource allocation failed. trace_info: w_compress::compress_file");
	}

	if (_ctx) LZ4F_freeCompressionContext(_ctx);//supports free on NULL
	if (_src) free(_src);
	if (_out_buffer) free(_out_buffer);

	return _result;
}
