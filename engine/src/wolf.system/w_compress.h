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

#if defined (__cplusplus)
extern "C" {
#endif

#include "w_system_export.h"
#include <stdio.h>
	
	typedef struct {
		int error;
		unsigned long long size_in;
		unsigned long long size_out;
	} w_compress_result;

	WSYS_EXP w_compress_result compress_file_c(FILE* pFileStreamIn, FILE* pCompressedFileOut, _Inout_ char* pErrorLog);
	//WSYS_EXP w_compress_result decompress_file_c(FILE* pCompressedFileIn, FILE* pFileStreamOut, char* pErrorLog);
	
#if defined (__cplusplus)
}
#endif

#endif //__W_COMPRESS_H__