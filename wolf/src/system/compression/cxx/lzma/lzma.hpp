#pragma once

#ifndef _7ZIP_H_
#define _7ZIP_H_

#include "rust/cxx.h"
#include <memory>

namespace lzma_cxx
{
	enum class LZMAType : ::std::uint8_t;

	/**
	 * compress stream using lzma1 algorithm
	 * @param p_src_buffer is the source buffer
	 * @param p_type type of LZMA algorithm
	 * @param p_level 0 <= level <= 9
	 * @param p_trace is the trace information
	 * @return compressed memory
	*/
	rust::Vec<uint8_t> compress(
		rust::Slice<const uint8_t> p_src_buffer,
		LZMAType p_type,
		uint32_t p_level,
		rust::String& p_trace);

	/**
	 * decompress stream using lzma1 algorithm
	 * @param p_src_buffer is the compressed source buffer
	 * @param p_type type of LZMA algorithm
	 * @param p_trace is the trace information
	 * @return decompressed memory
	*/
	rust::Vec<uint8_t> decompress(
		rust::Slice<const uint8_t> p_src_buffer,
		LZMAType p_type,
		rust::String& p_trace);

} // namespace lzma_cxx

#endif
