#include "w_compress.h"
#include "lz4/lz4.h"

#ifdef _WIN32
#include "w_compress_lzma.h"
#endif

W_RESULT w_compress_lz4(_In_       const char* pSrcBuffer,
                        _In_       w_compress_mode pMode,
                        _In_       int pAcceleration,
                        _Inout_    w_compress_result* pResult)
{
    if (!pResult || !pSrcBuffer || pResult->size_in == 0) return W_INVALIDARG;
    
    //allocate size for compressed data
    const int _max_dst_size = LZ4_compressBound((int)(pResult->size_in));
    pResult->data = malloc(_max_dst_size);
    if (!pResult->data)
    {
        W_ASSERT(false, "allocating memory for compressed buffer. trace info: w_compress_lz4");
        return W_FAILED;
    }

    int _compressed_buffer_size = -1;
    if (pMode == W_FAST)
    {
#ifdef __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif
        _compressed_buffer_size = LZ4_compress_fast_force(
            pSrcBuffer,
            pResult->data,
            pResult->size_in,
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
            return W_FAILED;
        }
    }
    else
    {
        W_ASSERT(false, "could not compress. trace info: compress_buffer_c");
        return W_FAILED;
    }

    return W_PASSED;
}

W_RESULT w_decompress_lz4(_In_      const char* pCompressedBuffer,
                          _Inout_   w_compress_result* pDecompressResult)
{
  if (!pCompressedBuffer || pDecompressResult->size_in == 0) return W_INVALIDARG;

    //allocate memory for decompress
    size_t _destination_capacity = pDecompressResult->size_in * 2;
    pDecompressResult->data = (char*)malloc(_destination_capacity);
    if (!pDecompressResult->data)
    {
         W_ASSERT(false, "error on allocate buffer for decompressed buffer. trace info: w_decompress_lz4");
        return W_FAILED;
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
        return W_FAILED;
    }
    else
    {
        pDecompressResult->size_out = _decompressed_size;
        pDecompressResult->data = (char*)realloc(pDecompressResult->data, pDecompressResult->size_out);
        if (!pDecompressResult->data)
        {
             W_ASSERT(false, "could not fit memory for de-compressed buffer. trace info: w_decompress_lz4");
            return W_FAILED;
        }
    }
    return W_PASSED;
}


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
