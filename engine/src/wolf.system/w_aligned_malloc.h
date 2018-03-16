/************************************************************************************//**
// Copyright (c) 2006-2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
// This is the interface of address-aligned malloc and free.
****************************************************************************************/
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _W_ALIGNED_MALLOC_H
#define _W_ALIGNED_MALLOC_H

#include "w_system_export.h"

//=================================================================================================================================
/// \brief Allocate a memory such that the starting address will be aligned to the specified alignment.
//=================================================================================================================================
WSYS_EXP W_FORCE_INLINE void* aligned_malloc(size_t bytes, size_t alignment);

//=================================================================================================================================
/// \brief Free the memory allocated by aligned_malloc().
//=================================================================================================================================
WSYS_EXP W_FORCE_INLINE void aligned_free(void* p);

#endif
