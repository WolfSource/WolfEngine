/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_compress.hpp
	Description		 : compress stream based on https://github.com/lz4/lz4
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

typedef enum
{
    W_DEFAULT,
    W_FAST
}
w_compress_mode;

typedef struct
{
    size_t                size_in;
    size_t                size_out;
    char*                 data;
} w_compress_result;

/**
 * compress stream using lz4 algorithm
 * @param pSrcBuffer is the source buffer
 * @param pCompressResult is the result of compress process
 * @param pMode can be W_DEFAULT or W_FAST
 * @param pAcceleration is the acceleration of process. the default value is 1
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_compress_lz4(_In_    const char* pSrcBuffer,
                        _In_	w_compress_mode pMode,
                        _In_	int pAcceleration,
                        _Inout_ w_compress_result* pCompressResult);

/**
 * compress stream using lz4 algorithm
 * @param pCompressedBuffer is the compressed source buffer
 * @param pDecompressInfo is the result of decompressed buffer
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_decompress_lz4(_In_	    const char* pCompressedBuffer,
                          _Inout_	w_compress_result* pDecompressInfo);

#ifdef W_PLATFORM_WIN

W_SYSTEM_EXPORT
W_RESULT w_compress_lzma(_In_ const uint8_t* pSrcBuffer,
                         _Inout_ w_compress_result* pCompressResult);

W_SYSTEM_EXPORT
W_RESULT w_decompress_lzma(_In_ const uint8_t* pCompressedBuffer,
                           _Inout_ w_compress_result* pDecompressInfo);
#endif

//TODO
		/*static w_compress_result compress_file(_In_ FILE* pFileStreamIn, _Inout_ FILE* pCompressedFileOut)
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
		}*/


#ifdef __cplusplus
}
#endif
