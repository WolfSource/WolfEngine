#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
	#include "apr_win32.h"
#elif defined(__APPLE__)
	#include "apr_osx.h"
#endif

#ifdef __cplusplus
}
#endif