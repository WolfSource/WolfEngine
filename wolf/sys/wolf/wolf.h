#pragma once

#ifndef W_EXPORT
#ifdef _MSC_VER
#define W_EXPORT __declspec(dllexport)
#else
#define W_EXPORT 
#endif
#endif

#include <stddef.h>