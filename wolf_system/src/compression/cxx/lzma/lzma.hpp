#pragma once

#ifndef _LZMA_H_
#define _LZMA_H_

#include "rust/cxx.h"
#include <memory>

namespace lzma_cxx
{
	/**
	 * compress stream using lzma1 algorithm
	 * @param pSrcBuffer is the source buffer
	 * @param pLevel 0 <= level <= 9
	 * @param pTrace is the trace information
	 * @return compressed memory
	*/
	rust::Vec<uint8_t> compress_1(rust::Slice<const uint8_t> pSrcBuffer,
								  uint32_t pLevel,
								  rust::String &pTrace);

	/**
	 * decompress stream using lzma1 algorithm
	 * @param pSrcBuffer is the compressed source buffer
     * @param pTrace is the trace information
	 * @return decompressed memory
	*/
	rust::Vec<uint8_t> decompress_1(rust::Slice<const uint8_t> pSrcBuffer,
									rust::String &pTrace);

	/**
	 * compress stream using lzma2 algorithm
	 * @param pSrcBuffer is the source buffer
	 * @param pLevel 0 <= level <= 9
	 * @param pTrace is the trace information
	 * @return compressed memory
	*/
	rust::Vec<uint8_t> compress_2(rust::Slice<const uint8_t> pSrcBuffer,
								  uint32_t pLevel,
								  rust::String &pTrace);

	/**
	 * decompress stream using lzma2 algorithm
	 * @param pSrcBuffer is the compressed source buffer
	 * @param pTrace is the trace information
	 * @return decompressed memory
	*/
	rust::Vec<uint8_t> decompress_2(rust::Slice<const uint8_t> pSrcBuffer,
									rust::String &pTrace);

} // namespace lzma_cxx

#endif
