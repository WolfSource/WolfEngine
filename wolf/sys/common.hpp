#pragma once

#include <concepts>
#include <gsl/gsl>
#include <mimalloc.h>

template <typename T>
requires std::is_base_of_v<w_buf, w_buf> || std::is_base_of_v<w_buf_t, w_buf_t>
bool is_valid_buffer(T p_buf)
{
    return (p_buf->len != 0 && p_buf->data != nullptr);
}