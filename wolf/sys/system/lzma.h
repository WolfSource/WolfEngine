            /*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifndef _LZMA_H_
#define _LZMA_H_

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef W_API
#ifdef _WIN32
#define W_API __declspec(dllexport)
#else
#define W_API
#endif
#endif

#include <Lzma2Dec.h>
#include <lzma2Enc.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>

#ifdef __cplusplus
}
#endif

#endif //_LZMA_H_
