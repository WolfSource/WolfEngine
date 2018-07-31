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

#pragma region Compress

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

#pragma endregion

#pragma region DeCompress

static int get_block_size(const LZ4F_frameInfo_t* pInfo, _Inout_ char* pErrorLog)
{
	switch (pInfo->blockSizeID)
	{
	case LZ4F_default:
	case LZ4F_max64KB:  return 1 << 16;
	case LZ4F_max256KB: return 1 << 18;
	case LZ4F_max1MB:   return 1 << 20;
	case LZ4F_max4MB:   return 1 << 22;
	default:
		snprintf(pErrorLog, strlen(pErrorLog),
			"Impossible with expected frame specification: %d. trace_info: w_compress::get_block_size",
			pInfo->blockSizeID);
		return -1;//return must be int
	}
}

//return: 1==error, 0==success
static int decompress_file_internal(
	FILE* f_in,
	FILE* f_out,
	LZ4F_dctx* dctx,
	////////////////////////////////////////////////
	char* src,
	////////////////////////////////////////////////
	size_t srcCapacity,
	size_t filled,
	size_t alreadyConsumed,
	////////////////////////////////////////////////
	char* dst,
	////////////////////////////////////////////////
	size_t dstCapacity,
	_Inout_ char* pErrorLog)
{
	int firstChunk = 1;
	size_t ret = 1;

	if (srcCapacity <= 0)
	{
		snprintf(pErrorLog, strlen(pErrorLog),
			"source capacity must be greater than zero. trace_info: w_compress::decompress_file_internal");
		return 1;
	}
	if (dstCapacity <= 0)
	{
		snprintf(pErrorLog, strlen(pErrorLog),
			"destination capacity must be greater than zero. trace_info: w_compress::decompress_file_internal");
		return 1;
	}
	if (filled > srcCapacity)
	{
		snprintf(pErrorLog, strlen(pErrorLog),
			"filled must be greater than source capacity. trace_info: w_compress::decompress_file_internal");
		return 1;
	}
	if (alreadyConsumed > filled)
	{
		snprintf(pErrorLog, strlen(pErrorLog),
			"already consumed must be greater than filled. trace_info: w_compress::decompress_file_internal");
		return 1;
	}	

	//Decompression
	while (ret != 0)
	{
		//Load more input
		size_t readSize = firstChunk ? filled : fread(src, 1, srcCapacity, f_in);
		firstChunk = 0;


		////////////////////////////////////////////////
		const char* srcPtr = src + alreadyConsumed;
		alreadyConsumed = 0;
		////////////////////////////////////////////////
		
		
		////////////////////////////////////////////////
		const char* const srcEnd = srcPtr + readSize;
		////////////////////////////////////////////////

		if (readSize == 0 || ferror(f_in)) 
		{
			snprintf(pErrorLog, strlen(pErrorLog),
				"error happened while decompressing. error on reading source file. trace_info: w_compress::decompress_file_internal");
			return 1;
		}

		/* 
			Continue while there is more input to read (srcPtr != srcEnd)
			and the frame isn't over (ret != 0)
		 */
		while (srcPtr != srcEnd && ret != 0) 
		{
			/* Any data within dst has been flushed at this stage */
			size_t dstSize = dstCapacity;
			size_t srcSize = srcEnd - srcPtr;
			ret = LZ4F_decompress(dctx, dst, &dstSize, srcPtr, &srcSize, /* LZ4F_decompressOptions_t */ NULL);
			if (LZ4F_isError(ret))
			{
				snprintf(pErrorLog, strlen(pErrorLog),
					"error happened while decompressing. error: %s. trace_info: w_compress::decompress_file_internal", LZ4F_getErrorName(ret));
				return 1;
			}
			
			//Flush output */
			if (dstSize != 0) safe_fwrite(dst, 1, dstSize, f_out, pErrorLog);
		
			//Update input
			srcPtr += srcSize;
		}
	}

	{
		size_t const readSize = fread(src, 1, 1, f_in);
		if (readSize != 0 || !feof(f_in))
		{
			snprintf(pErrorLog, strlen(pErrorLog),
				"Trailing data left in file after frame. trace_info: w_compress::decompress_file_internal");
			return 1;
		}
	}

	return 0;
}

//return: 1==error, 0==completed
static int decompress_file_alloc_dst(
	FILE* pFileIn,
	FILE* pFileOut,
	LZ4F_dctx* pDecompressContext,
	void* pSrc,
	size_t pSrcCapacity,
	_Inout_ char* pErrorLog)
{
	if (pSrcCapacity < LZ4F_HEADER_SIZE_MAX)
	{
		snprintf(pErrorLog, strlen(pErrorLog),
			"ensure LZ4F_getFrameInfo() can read enough data. trace_info: w_compress::decompress_file_alloc_dst");
		return 1;
	}

	//Read frame header
	size_t const _read_size = fread(pSrc, 1, pSrcCapacity, pFileIn);
	if (_read_size == 0 || ferror(pFileIn))
	{
		snprintf(pErrorLog, strlen(pErrorLog),
			"not enough input or error reading file. trace_info: w_compress::decompress_file_alloc_dst");
		return 1;
	}

	LZ4F_frameInfo_t info;
	size_t _consumed_size = _read_size;
	{
		size_t const _frames = LZ4F_getFrameInfo(pDecompressContext, &info, pSrc, &_consumed_size);
		if (LZ4F_isError(_frames))
		{
			snprintf(pErrorLog, strlen(pErrorLog),
				"get frame info error happened : %s. trace_info: w_compress::decompress_file_alloc_dst",
				LZ4F_getErrorName(_frames));
			return 1;
		}
	}

	/*
		Allocating enough space for an entire block isn't necessary for
		correctness, but it allows some memcpy's to be elided.
	*/
	size_t _dst_capacity = 0;
	auto _block_size = get_block_size(&info, pErrorLog);
	if (_block_size == -1)
	{
		return 1;
	}

	_dst_capacity = _block_size;

	void* const _dst = malloc(_dst_capacity);
	if (!_dst)
	{
		snprintf(pErrorLog, strlen(pErrorLog),
			"allocating memory for decompressed file. trace_info: w_compress::decompress_file_alloc_dst");
		return 1;
	}

	int const _decompression_result = decompress_file_internal(
		pFileIn,
		pFileOut,
		pDecompressContext,
		pSrc,
		pSrcCapacity,
		_read_size,
		_consumed_size,
		_dst,
		_dst_capacity,
		pErrorLog);

	free(_dst);
	return _decompression_result;
}

w_compress_result decompress_file_c(FILE* pCompressedFileIn, FILE* pFileStreamOut, _Inout_ char* pErrorLog)
{
	w_compress_result _result = { 1, 0, 0 };
	if (!pCompressedFileIn || !pFileStreamOut)
	{
		return _result;
	}

	//Ressource allocation
	void* const _src = malloc(IN_CHUNK_SIZE);
	if (!_src)
	{
		snprintf(pErrorLog, strlen(pErrorLog), "allocation resource for decompressing file. trace_info: w_compress::decompress_file_c");
		return _result;
	}

	LZ4F_dctx* _dctx;
	{
		size_t const _dctx_status = LZ4F_createDecompressionContext(&_dctx, LZ4F_VERSION);
		if (LZ4F_isError(_dctx_status))
		{
			snprintf(pErrorLog, strlen(pErrorLog),
				"LZ4F_dctx creation error: %s. trace_info: w_compress::decompress_file_c", LZ4F_getErrorName(_dctx_status));
		}
	}

	if (_dctx)
	{
		_result.error = decompress_file_alloc_dst(pCompressedFileIn, pFileStreamOut, _dctx, _src, IN_CHUNK_SIZE, pErrorLog);
		LZ4F_freeDecompressionContext(_dctx);
	}

	free(_src);
	return _result;
}

#pragma endregion