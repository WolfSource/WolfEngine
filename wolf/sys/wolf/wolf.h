#pragma once

#ifndef W_EXPORT
#ifdef _MSC_VER
#define W_EXPORT __declspec(dllexport)
#pragma warning(disable : 4471) // forward enum declaration
#else
#define W_EXPORT 
#endif
#endif

#include <stddef.h>
#include <stdint.h>

#define W_MAX_PATH 260