#pragma once

#ifdef WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

//#define WOLF_API __declspec(dllexport)

// #else

// #define WOLF_API

#endif

namespace wolf
{
	auto w_print_version() -> void;
} // namespace wolf