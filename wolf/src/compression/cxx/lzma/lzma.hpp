#pragma once

#include "rust/cxx.h"
#include <memory>

namespace lzma_cxx
{
    struct LZMAResult;

    /**
	 * compress stream using lzma algorithm
	 * @param pSrcBuffer is the source buffer
	 * @param pResult is the result of compress process
	 * @return result
	*/
    bool compress(rust::Slice<const uint8_t> pSrcBuffer,
                  LZMAResult &pResult);

    /**
	* decompress stream using lzma algorithm
	* @param pSrcBuffer is the compressed source buffer
	* @param pResult is the result of decompressed buffer
	* @return result
	*/
    bool decompress(rust::Slice<const uint8_t> pSrcBuffer,
                    LZMAResult &pResult);
} // namespace lzma_cxx
