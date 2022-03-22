#pragma once

#ifndef _LZ4_H_
#define _LZ4_H_

#include "rust/cxx.h"
#include <memory>

namespace lz4_cxx
{
    /**
     * compress stream using lz4 algorithm
     * @param pSrcBuffer is the source buffer
     * @param pFastMode true means Fast mode and zero is Default mode
     * @param pAcceleration is the acceleration of process. the default value is 1
     * @param pTrace is the trace information
     * @return compressed memory
    */
    rust::Vec<uint8_t> compress(
        rust::Slice<const uint8_t> pSrcBuffer,
        bool pFastMode,
        uint32_t pAcceleration,
        rust::String& pTrace);

    /**
     * decompress stream using lz4 algorithm
     * @param pSrcBuffer is the compressed source buffer
     * @param pTrace is the trace information
     * @return decompressed memory
    */
    rust::Vec<uint8_t> decompress(
        rust::Slice<const uint8_t> pSrcBuffer,
        rust::String& pTrace);

} // namespace lz4_cxx

#endif