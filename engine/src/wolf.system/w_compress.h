/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_compress.h
	Description		 : compress stream based on https://github.com/lz4/lz4
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_COMPRESS_H__
#define __W_COMPRESS_H__

#include "w_system_export.h"
#include <lz4/lz4.h>
#include <lz4/lz4frame.h>

namespace wolf
{
	namespace system
	{
		typedef struct
		{
			int error;
			unsigned long long size_in;
			unsigned long long size_out;
		} w_compress_result;

		class w_compress
		{
		public:
			WSYS_EXP static w_compress_result compress_file(_In_ FILE* pFileStreamIn, _Inout_ FILE* pCompressedFileOut);
			WSYS_EXP static w_compress_result decompress_file(_In_ FILE* pCompressedFileIn, _Inout_ FILE* pFileStreamOut);
		};
	}
}

#endif //__W_COMPRESS_H__