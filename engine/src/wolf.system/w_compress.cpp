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

	if (pOutCapacity < LZ4F_HEADER_SIZE_MAX)
	{
		logger.error("output capacity must be greater or equal to LZ4F_HEADER_SIZE_MAX");
		return _result;
	}
	if (pOutCapacity < LZ4F_compressBound(pInChunkSize, &sPrefs))
	{
		logger.error("output capacity be greater or equal to LZ4F_compressBound");
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

	//stream file and compress it
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

static size_t _get_block_size(const LZ4F_frameInfo_t* info)
{
	switch (info->blockSizeID)
	{
	case LZ4F_default:
	case LZ4F_max64KB:  return 1 << 16;
	case LZ4F_max256KB: return 1 << 18;
	case LZ4F_max1MB:   return 1 << 20;
	case LZ4F_max4MB:   return 1 << 22;
	default:
		logger.error("Impossible with expected frame specification: {} . trace info: w_compress::_get_block_size", info->blockSizeID);
	}
}

// return : 1==error, 0==success
static int decompress_file_internal(
	FILE* pFileIn, FILE* pFileOut,
	LZ4F_dctx* dctx,
	void* src, 
	size_t srcCapacity, 
	size_t filled, 
	size_t alreadyConsumed,
	void* dst, 
	size_t dstCapacity)
{
	int firstChunk = 1;
	size_t _result = 1;

	if (srcCapacity <= 0 || filled > srcCapacity || alreadyConsumed > filled || dstCapacity <= 0)
	{
		return _result;
	}


	///* Decompression */
	//while (_result != 0) 
	//{
	//	/* Load more input */
	//	size_t readSize = firstChunk ? filled : fread(src, 1, srcCapacity, f_in); firstChunk = 0;
	//	const void* srcPtr = src + alreadyConsumed; alreadyConsumed = 0;
	//	const void* const srcEnd = srcPtr + readSize;
	//	if (readSize == 0 || ferror(f_in)) {
	//		printf("Decompress: not enough input or error reading file\n");
	//		return 1;
	//	}

	//	/* Decompress:
	//	* Continue while there is more input to read (srcPtr != srcEnd)
	//	* and the frame isn't over (ret != 0)
	//	*/
	//	while (srcPtr != srcEnd && _result != 0)
	//	{
	//		/* Any data within dst has been flushed at this stage */
	//		size_t dstSize = dstCapacity;
	//		size_t srcSize = srcEnd - srcPtr;
	//		_result = LZ4F_decompress(dctx, dst, &dstSize, srcPtr, &srcSize, /* LZ4F_decompressOptions_t */ NULL);
	//		if (LZ4F_isError(_result))
	//		{
	//			printf("Decompression error: %s\n", LZ4F_getErrorName(_result));
	//			return 1;
	//		}
	//		/* Flush output */
	//		if (dstSize != 0) safe_fwrite(dst, 1, dstSize, f_out);
	//		/* Update input */
	//		srcPtr += srcSize;
	//	}
	//}

	///* Check that there isn't trailing input data after the frame.
	//* It is valid to have multiple frames in the same file,
	//* but this example only supports one frame.
	//*/
	//{   size_t const readSize = fread(src, 1, 1, f_in);
	//if (readSize != 0 || !feof(f_in)) {
	//	printf("Decompress: Trailing data left in file after frame\n");
	//	return 1;
	//}   }

	return 0;
}

// return 1 == error, 0 == completed
static int decompress_file_alloc_dst(
	FILE* pFileIn,
	FILE* pFileOut,
	LZ4F_dctx* pDecompressContext,
	void* pSrc,
	size_t pSrcCapacity)
{
	if (!pDecompressContext || pSrcCapacity < LZ4F_HEADER_SIZE_MAX) return 1;

	//Read Frame header
	size_t const _read_size = fread(pSrc, 1, pSrcCapacity, pFileIn);
	if (_read_size == 0 || ferror(pFileIn))
	{
		logger.error("not enough input or error reading file. trace info: w_compress::decompress_file_alloc_dst");
		return 1;
	}

	LZ4F_frameInfo_t _info;
	size_t _consumed_size = _read_size;
	{
		size_t const _frame_info = LZ4F_getFrameInfo(pDecompressContext, &_info, pSrc, &_consumed_size);
		if (LZ4F_isError(_frame_info))
		{
			logger.error("LZ4F_getFrameInfo error: {}. trace info: w_compress::decompress_file_alloc_dst", LZ4F_getErrorName(_frame_info));
			return 1;
		}
	}

	//Allocating enough space for an entire block isn't necessary for correctness, but it allows some memcpy's to be elided
	size_t const _dst_capacity = _get_block_size(&_info);
	void* const _dst = malloc(_dst_capacity);
	if (!_dst)
	{
		logger.error("decompress_file destination");
		return 1;
	}

	int const _result = decompress_file_internal(
		pFileIn,
		pFileOut,
		pDecompressContext,
		pSrc,
		pSrcCapacity,
		_read_size,
		_consumed_size,
		_dst,
		_dst_capacity);

	free(_dst);

	return _result;
}

w_compress_result w_compress::decompress_file(_In_ FILE* pCompressedFileIn, _Inout_ FILE* pFileStreamOut)
{
	w_compress_result _result = { 1, 0, 0 };//error by default

	if (!pCompressedFileIn || !pFileStreamOut) return _result;

	// Ressource allocation
	void* const _src = malloc(IN_CHUNK_SIZE);
	if (!_src) 
	{ 
		logger.error("could not allocate resources for decompress file. trace infor: w_compress::decompress_file"); 
		return _result;
	}

	LZ4F_dctx* _decompress_ctx;
	{
		size_t const _dctx_status = LZ4F_createDecompressionContext(&_decompress_ctx, LZ4F_VERSION);
		if (LZ4F_isError(_dctx_status))
		{
			logger.error("LZ4F decompression context creation error: {}. trace infor: w_compress::decompress_file", LZ4F_getErrorName(_dctx_status));
		}
		else
		{
			_result.error = !_decompress_ctx ? 1 /* error */ : decompress_file_alloc_dst(pCompressedFileIn, pFileStreamOut, _decompress_ctx, _src, IN_CHUNK_SIZE);
		}
	}

	free(_src);
	LZ4F_freeDecompressionContext(_decompress_ctx);   /* note : free works on NULL */
	
	return _result;
}
