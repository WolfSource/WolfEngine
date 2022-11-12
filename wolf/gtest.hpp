#pragma once

#ifdef WOLF_TESTS

#ifdef __SANITIZE_ADDRESS__
#define _DISABLE_VECTOR_ANNOTATION
#endif

#include "DISABLE_ANALYSIS_BEGIN"
#ifdef _MSC_VER
#pragma warning (disable: 26440)
#endif // #ifdef _MSC_VER
#include <gtest/gtest.h>
#include "DISABLE_ANALYSIS_END"

#endif

#include <system/w_leak_detector.hpp>
