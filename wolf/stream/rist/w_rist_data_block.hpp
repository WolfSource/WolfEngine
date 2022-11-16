﻿/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_RIST

#pragma once

#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
#include <librist/librist.h>
#include <rist-private.h>
#include <DISABLE_ANALYSIS_END>

namespace wolf::stream::rist {
class w_rist_data_block {
  friend class w_rist;

public:
  W_API w_rist_data_block() noexcept;
  W_API virtual ~w_rist_data_block();

  // move constructor.
  W_API w_rist_data_block(w_rist_data_block &&p_other) noexcept = default;
  // move constructor.
  W_API w_rist_data_block(rist_data_block &&p_other) noexcept;

  // move assignment operator.
  W_API w_rist_data_block &
  operator=(w_rist_data_block &&p_other) noexcept = default;

  /**
   * set data to rist data block
   * @param p_data, data and size of rist
   * @returns result
   */
  W_API void set(_In_ const std::tuple<const void *, size_t> &p_data) noexcept;

  /**
   * get data of rist data block
   * @returns result of data with size
   */
  W_API std::tuple<const void *, size_t> get() const;

  /**
   * get data block as string
   * @returns result of data with size
   */
  std::string to_string() const;

private:
  // copy constructor
  w_rist_data_block(const w_rist_data_block &) = delete;
  // copy operator
  w_rist_data_block &operator=(const w_rist_data_block &) = delete;

  rist_data_block *_block = nullptr;
};
} // namespace wolf::stream::rist

#endif