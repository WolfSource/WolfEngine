/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __SANITIZE_ADDRESS__
#define _DISABLE_VECTOR_ANNOTATION
#endif

#ifdef _MSC_VER
#include <codeanalysis\warnings.h>
#endif

#ifdef WIN32

#include <Windows.h>
#include <format>

#endif

#ifndef EMSCRIPTEN
#include <filesystem>
#endif

#include <chrono>
#include <exception>
#include <memory>
#include <sstream>
#include <string>

#include "DISABLE_ANALYSIS_BEGIN"

#include <boost/leaf.hpp>
#include <gsl/gsl>

#ifdef WOLF_SYSTEM_MIMALLOC

#include <mimalloc-override.h>

#endif

#ifdef WOLF_SYSTEM_STACKTRACE
    // disable warning 26458
    #ifdef _MSC_VER
        #pragma warning (disable:26458)
    #endif //_MSC_VER
    #include <boost/stacktrace/stacktrace.hpp>
#endif
#include "DISABLE_ANALYSIS_END"

constexpr auto W_MAX_PATH = 260;
constexpr auto W_MAX_BUFFER_SIZE = 1024;

using defer = std::shared_ptr<void>;

struct w_buffer {

  w_buffer() noexcept = default;

  w_buffer(std::array<char, W_MAX_BUFFER_SIZE> &&p_array,
           size_t &p_used_bytes) noexcept {
    this->buf = std::move(p_array);
    this->used_bytes = p_used_bytes;
  }

  w_buffer(const std::string_view &p_str) { from_string(p_str); }

  void from_string(const std::string_view p_str) {
    const auto _size = p_str.size();
    this->used_bytes = _size > 1024 ? 1024 : _size;
    std::copy(p_str.cbegin(), p_str.cbegin() + this->used_bytes, buf.begin());
  }

  std::string to_string() { return std::string(this->buf.data(), this->used_bytes); }

  std::array<char, W_MAX_BUFFER_SIZE> buf = {0};
  size_t used_bytes = 0;
};

//#ifdef __clang__
//#define W_ALIGNMENT_16 __attribute__((packed)) __attribute__((aligned(16)))
//#define W_ALIGNMENT_32 __attribute__((packed)) __attribute__((aligned(32)))
//#define W_ALIGNMENT_64 __attribute__((packed)) __attribute__((aligned(64)))
//#define W_ALIGNMENT_128 __attribute__((packed)) __attribute__((aligned(128)))
//#else
//#define W_ALIGNMENT_16
//#define W_ALIGNMENT_32
//#define W_ALIGNMENT_64
//#define W_ALIGNMENT_128
//#endif

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

namespace wolf {
/**
 * returns wolf version
 * @return string format with the following style
 * "<major>.<minor>.<patch>.<debug>"
 */
W_API std::string w_init();
} // namespace wolf
