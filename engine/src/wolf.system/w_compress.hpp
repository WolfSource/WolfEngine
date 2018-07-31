/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_compress.hpp
	Description		 : compress stream based on https://github.com/lz4/lz4
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_COMPRESS_HPP__
#define __W_COMPRESS_HPP__

#include "w_compress.h"
#include "w_system_export.h"
#include "w_logger.h"

namespace wolf
{
	namespace system
	{
		struct w_compress
		{
			static W_RESULT compress_buffer(
				_In_		char* pSrcBuffer,
				_Inout_		w_compress_result* pCompressResult,
				_In_		w_compress_mode pMode = w_compress_mode::W_COMPRESS_DEFAULT,
				_In_		int pAcceleration = 1.0)
			{
				if (!pCompressResult) return W_RESULT::W_INVALIDARG;

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
}

#endif //__W_COMPRESS_HPP__