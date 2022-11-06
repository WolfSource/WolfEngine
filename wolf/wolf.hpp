/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef _MSC_VER
#include <codeanalysis\warnings.h>
#endif

#ifdef WIN32

#include <Windows.h>

#endif

#ifndef EMSCRIPTEN
#include <filesystem>
#endif

#include <format>
#include <memory>
#include <sstream>
#include <string>

#include "DISABLE_ANALYSIS_BEGIN"

#include <boost/leaf.hpp>
#include <gsl/gsl>

#ifdef WOLF_SYSTEM_STACKTRACE
    // disable warning 26458
    #ifdef _MSC_VER
        #pragma warning (disable:26458)
    #endif //_MSC_VER
    #include <boost/stacktrace/stacktrace.hpp>
#endif
#include "DISABLE_ANALYSIS_END"

constexpr auto W_MAX_PATH = 260;
constexpr auto W_MAX_SIZE_ON_STACK = 4096;

using defer = std::shared_ptr<void>;

#ifdef __clang__
#define W_ALIGNMENT_16 __attribute__((packed)) __attribute__((aligned(16)))
#define W_ALIGNMENT_32 __attribute__((packed)) __attribute__((aligned(32)))
#define W_ALIGNMENT_64 __attribute__((packed)) __attribute__((aligned(64)))
#define W_ALIGNMENT_128 __attribute__((packed)) __attribute__((aligned(128)))
#else
#define W_ALIGNMENT_16
#define W_ALIGNMENT_32
#define W_ALIGNMENT_64
#define W_ALIGNMENT_128
#endif

#ifdef _MSC_VER

#define W_API __declspec(dllexport)
#define ASM __asm

#else

#define W_API
#define ASM __asm__

//define dummy SAL
#define _In_
#define _In_z_
#define _Out_
#define _Inout_
#define _Inout_z_
#define _In_opt_
#define _In_opt_z_
#define _Inout_opt_

#endif

#include <system/w_trace.hpp>

namespace wolf
{
    /**
     * returns wolf version
     * @return string format with the following style
     * "<major>.<minor>.<patch>.<debug>"
     */
    W_API std::string w_init();

    namespace system
    {
#ifndef EMSCRIPTEN
        /**
         * get current exe path
         * @returns current exe path
         */
        W_API boost::leaf::result<std::filesystem::path> w_current_process_path();
#endif // EMSCRIPTEN
    } // namespace system
} // namespace wolf
