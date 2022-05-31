#pragma once

#ifndef W_EXPORT
#ifdef _MSC_VER
#define C_EXPORT export extern "C" __declspec(dllexport)
#else
#define C_EXPORT export extern "C"
#endif
#endif

//W_EXPORT
//struct w_cbuf {
//  char *data = nullptr;
//  size_t len = 0;
//};
//
//W_EXPORT
//struct w_ubuf {
//  uint8_t *data = nullptr;
//  size_t len = 0;
//};
//
//template <typename T>
//requires std::is_base_of_v<w_cbuf, w_cbuf> || std::is_base_of_v<w_ubuf, w_ubuf>
//bool is_valid_buffer(T p_buf) {
//  return (p_buf && p_buf->len != 0 && p_buf->data != nullptr);
//}