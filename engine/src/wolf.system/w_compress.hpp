/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_compress.hpp
	Description		 : compress stream based on https://github.com/lz4/lz4
	Comment          :
*/

#pragma once

#include "w_compress_lz4.h"
#include "w_system_export.h"
#include "w_logger.h"

#ifdef __WIN32
#include "w_compress_lzma.h"
#endif

namespace wolf::system
{
	struct w_compress
	{
		static W_RESULT compress_lz4(
			_In_		const char* pSrcBuffer,
			_Inout_		w_compress_result* pCompressResult,
			_In_		w_compress_mode pMode = w_compress_mode::W_DEFAULT,
			_In_		int pAcceleration = 1.0)
		{
			if (!pCompressResult || !pSrcBuffer) return W_RESULT::W_INVALIDARG;

			W_RESULT _result = W_PASSED;

			auto _err_log = (char*)malloc(256 * sizeof(char));
			if (compress_buffer_c(
				pSrcBuffer,
				pMode,
				pAcceleration,
				pCompressResult,
				_err_log))
			{
				logger.error(_err_log);
				_result = W_FAILED;
			}
			free(_err_log);

			return _result;
		}

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
	};
}
