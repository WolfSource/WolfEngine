/************************************************************************************//**
// Copyright (c) 2006-2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
// This is the interface of address-aligned malloc and free.
****************************************************************************************/

#pragma once

#include "w_system_export.h"

#if defined(_WIN32) && (defined(_MSC_VER) || defined(__INTEL_COMPILER) || defined(__clang__)) // Windows: MSVC / Intel compiler / clang

#ifndef W_FORCE_INLINE
	#define W_FORCE_INLINE __forceinline
#endif

#elif defined(__GNUG__) || defined(__clang__) // G++ or clang
#ifndef W_FORCE_INLINE
	#define W_FORCE_INLINE inline
#endif
#else
#error Unsupported compiler
#endif

//=================================================================================================================================
/// \brief Allocate a memory such that the starting address will be aligned to the specified alignment.
//=================================================================================================================================
WSYS_EXP W_FORCE_INLINE void* aligned_malloc(size_t bytes, size_t alignment);

//=================================================================================================================================
/// \brief Free the memory allocated by aligned_malloc().
//=================================================================================================================================
WSYS_EXP W_FORCE_INLINE void aligned_free(void* p);

