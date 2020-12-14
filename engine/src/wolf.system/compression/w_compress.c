#include "w_compress.h"

#include <stdlib.h>
#include <apr.h>
#include <apr-1/apr_general.h>
#include <apr-1/apr_strings.h>
#include <apr-1/apr_tables.h>
#include <apr-1/apr_file_io.h>
#include <apr-1/apr_hash.h>
#include "log/w_log.h"

#include "lz4/lz4.h"

#ifdef W_PLATFORM_WIN
#include "lzma/LzmaEnc.h"
#include "lzma/LzmaDec.h"
#endif

W_RESULT w_compress_lz4(_In_       const char* pSrcBuffer,
                        _In_       w_compress_mode pMode,
                        _In_       int pAcceleration,
                        _Inout_    w_compress_result* pResult)
{
    if (!pResult || !pSrcBuffer || pResult->size_in == 0) return APR_BADARG;
    
    //allocate size for compressed data
    const int _max_dst_size = LZ4_compressBound((int)(pResult->size_in));
    pResult->data = (char*)malloc(_max_dst_size);
    if (!pResult->data)
    {
        W_ASSERT(false, "allocating memory for compressed buffer. trace info: w_compress_lz4");
        return W_FAILURE;
    }

    int _compressed_buffer_size = -1;
    if (pMode == W_FAST)
    {
#ifdef __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif

		_compressed_buffer_size =
			LZ4_compress_fast(
				pSrcBuffer,
				pResult->data,
				(int)pResult->size_in,
				_max_dst_size,
				pAcceleration);
#ifdef __clang__
        #pragma clang diagnostic pop
#endif
    }
    else
    {
        _compressed_buffer_size = LZ4_compress_default(
            pSrcBuffer,
            pResult->data,
            (int)pResult->size_in,
            _max_dst_size);
    }

    if (_compressed_buffer_size > 0)
    {
        //realloc compress_data to free up memory
        pResult->size_out = (size_t)_compressed_buffer_size;
        pResult->data = (char*)realloc(pResult->data, pResult->size_out);
        if (!pResult->data)
        {
            W_ASSERT(false, "could not fit memory of compressed buffer. trace info: compress_buffer_c");
            return W_FAILURE;
        }
    }
    else
    {
        W_ASSERT(false, "could not compress. trace info: compress_buffer_c");
        return W_FAILURE;
    }

    return W_SUCCESS;
}

W_RESULT w_decompress_lz4(_In_      const char* pCompressedBuffer,
                          _Inout_   w_compress_result* pDecompressResult)
{
  if (!pCompressedBuffer || pDecompressResult->size_in == 0) return APR_BADARG;

    //allocate memory for decompress
    size_t _destination_capacity = pDecompressResult->size_in * 2;
    pDecompressResult->data = (char*)malloc(_destination_capacity);
    if (!pDecompressResult->data)
    {
         W_ASSERT(false, "error on allocate buffer for decompressed buffer. trace info: w_decompress_lz4");
        return W_FAILURE;
    }

    int _decompressed_size = -1;
    int _number_of_try = 10;
    
    while (_number_of_try > 0)
    {
        _decompressed_size = LZ4_decompress_safe(
                pCompressedBuffer,
                &pDecompressResult->data[0],
                (int)pDecompressResult->size_in,
                (int)_destination_capacity);
        if (_decompressed_size >= 0)
        {
            break;
        }
        else
        {
            _destination_capacity *= 2;
            pDecompressResult->data = (char*)realloc(pDecompressResult->data, _destination_capacity);
            if (!pDecompressResult->data)
            {
                W_ASSERT(false, "could not re-allocate memory of de-compressed buffer. trace info: w_decompress_lz4");
                break;
            }
        }

        _number_of_try--;
    }

    if (_decompressed_size <= 0)
    {
        W_ASSERT(false, "decompress size must be greater than zero. trace info: w_decompress_lz4");
        free(pDecompressResult->data);
        pDecompressResult->data = NULL;
        pDecompressResult->size_out = 0;
        return W_FAILURE;
    }
    else
    {
        pDecompressResult->size_out = _decompressed_size;
        pDecompressResult->data = (char*)realloc(pDecompressResult->data, pDecompressResult->size_out);
        if (!pDecompressResult->data)
        {
             W_ASSERT(false, "could not fit memory for de-compressed buffer. trace info: w_decompress_lz4");
            return W_FAILURE;
        }
    }
    return W_SUCCESS;
}

#ifdef W_PLATFORM_WIN

static void* _lzma_alloc(ISzAllocPtr pPtr, size_t pSize)
{
	return malloc(pSize);
}
static void _lzma_free(ISzAllocPtr pPtr, void* pAddr)
{
	free(pAddr);
}

static ISzAlloc _alloc_funcs = { _lzma_alloc, _lzma_free };

int w_compress_lzma(_In_ const uint8_t* pSourceBuffer, _Inout_ w_compress_result* pCompressInfo)
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
		_props.dictSize = (UINT32)pCompressInfo->size_in; //smaller dictionary = faster!
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
	SizeT _output_size_64 = pCompressInfo->size_in * 2;
	if (_output_size_64 < 1024)
	{
		_output_size_64 = 1024;
	}

	uint8_t* _compress_mem = (uint8_t*)malloc(_output_size_64 * sizeof(uint8_t));
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
		if (pCompressInfo->data)
		{
			memcpy(pCompressInfo->data, _props_encoded, 5);
			for (int i = 0; i < 8; i++)
			{
				pCompressInfo->data[5 + i] = (pCompressInfo->size_in >> (i * 8)) & 0xFF;
			}
			memcpy(pCompressInfo->data + 13, _compress_mem, _output_size_64);
		}
		return 0;
	}
	return 1;
}

int w_decompress_lzma(_In_ const uint8_t* pCompressedBuffer, _Inout_ w_compress_result* pDeCompressInfo)
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

#endif

/*

		static W_RESULT decompress_lz4(
			_In_	const char* pCompressedBuffer,
			_Inout_	w_compress_result* pDecompressInfo)
		{
			if (!pDecompressInfo || !pCompressedBuffer) return W_RESULT::W_INVALIDARG;

			W_RESULT _result = W_PASSED;

			auto _err_log = (char*)malloc(256 * sizeof(char));
			if (decompress_buffer_c(
				pCompressedBuffer,
				pDecompressInfo,
				_err_log))
			{
				logger.error(_err_log);
				_result = W_FAILED;
			}
			free(_err_log);

			return _result;
		}

#ifdef __WIN32
		static W_RESULT compress_lzma(
			_In_ const uint8_t* pSrcBuffer,
			_Inout_ w_compress_result* pCompressResult)
		{
			if (!pCompressResult || !pSrcBuffer) return W_RESULT::W_INVALIDARG;

			W_RESULT _result = W_PASSED;

			if (w_compress_lzma::compress(
				pSrcBuffer,
				pCompressResult))
			{
				_result = W_FAILED;
			}

			return _result;
		}

		static W_RESULT decompress_lzma(
			_In_ const uint8_t* pCompressedBuffer,
			_Inout_ w_compress_result* pDecompressInfo)
		{
			if (!pDecompressInfo || !pCompressedBuffer) return W_RESULT::W_INVALIDARG;

			W_RESULT _result = W_PASSED;

			if (w_compress_lzma::decompress(
				pCompressedBuffer,
				pDecompressInfo))
			{
				_result = W_FAILED;
			}

			return _result;
		}
#endif

		static w_compress_result compress_file(_In_ FILE* pFileStreamIn, _Inout_ FILE* pCompressedFileOut)
		{
			auto _err_log = (char*)malloc(256 * sizeof(char));
			auto _result = compress_file_c(pFileStreamIn, pCompressedFileOut, _err_log);
			if (_result.error)
			{
				logger.error(_err_log);
			}
			free(_err_log);

			if (pFileStreamIn) fclose(pFileStreamIn);
			if (pCompressedFileOut) fclose(pCompressedFileOut);

			return _result;
		}
		static w_compress_result decompress_file(_In_ FILE* pCompressedFileIn, _Inout_ FILE* pFileStreamOut)
		{
			auto _err_log = (char*)malloc(256 * sizeof(char));
			auto _result = decompress_file_c(pCompressedFileIn, pFileStreamOut, _err_log);
			if (_result.error)
			{
				logger.error(_err_log);
			}
			free(_err_log);

			if (pCompressedFileIn) fclose(pCompressedFileIn);
			if (pFileStreamOut) fclose(pFileStreamOut);

			return _result;
		}
	};
}
*/
