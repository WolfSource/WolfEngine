#pragma once

#include <span>
#include <string_view>

namespace wolf::stream::quic {

using w_alpn_view = std::span<std::uint8_t>;

inline w_alpn_view as_alpn_view(w_alpn_view p_alpn_view)
{
    return p_alpn_view;
}

inline w_alpn_view as_alpn_view(const std::string_view p_str)
{
    return w_alpn_view((uint8_t*)p_str.data(), p_str.length());
}

}  // namespace wolf::stream::quic
