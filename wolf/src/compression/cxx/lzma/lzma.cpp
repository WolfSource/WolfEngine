#include "lzma.hpp"
#include "wolf/src/compression/lzma.rs.h"

namespace lzma_cxx
{
    bool compress(
        rust::Slice<const uint8_t> pSrcBuffer,
        LZMAResult &pResult)
    {
        return true;
    }

    bool decompress(
        rust::Slice<const uint8_t> pSrcBuffer,
        LZMAResult &pResult)
    {
        return true;
    }
}