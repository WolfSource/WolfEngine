/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_compress_lzma.h
	Description		 : compress stream based on https://www.7-zip.org/sdk.html
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_COMPRESS_LZMA_H__
#define __W_COMPRESS_LZMA_H__

#include "w_system_export.h"
#include "w_compress_data_type.h"
#include "w_std.h"

#ifdef __WIN32
#include "lzma/LzmaEnc.h"
#include "lzma/LzmaDec.h"
#endif

static void* _lzma_alloc(ISzAllocPtr, size_t size)
{
	return new uint8_t[size];
}
static void _lzma_free(ISzAllocPtr, void *addr)
{
	if (!addr) return;

	delete[] reinterpret_cast<uint8_t *>(addr);
}

static ISzAlloc _alloc_funcs = { _lzma_alloc, _lzma_free };

namespace wolf
{
	namespace system
	{
		struct w_compress_lzma
		{
			WSYS_EXP static int compress(const uint8_t* pSourceBuffer, w_compress_result* pCompressInfo);
			WSYS_EXP static int decompress(const uint8_t* pCompressedBuffer, w_compress_result* pDeCompressInfo);
		};
	}
}

#endif //__W_COMPRESS_LZMA_H__
