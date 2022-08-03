#include "w_gametime.hpp"

using w_gametime = wolf::system::w_gametime;

void w_gametime::reset() noexcept
{
  this->_last_time = std::chrono::steady_clock::now();
  this->_left_over_ticks = 0.0;
  this->_fps = 0;
  this->_frames_this_sec = 0;
  this->_secs_counter = 0.0;
}

[[nodiscard]] constexpr auto w_gametime::get_elapsed_secs() noexcept
{
  return this->_elapsed_secs;
}

[[nodiscard]] constexpr auto w_gametime::get_total_secs() noexcept
{
  return this->_total_secs;
}

[[nodiscard]] auto w_gametime::get_frames_count() const noexcept
{
  return this->_frames_count;
}

[[nodiscard]] auto w_gametime::get_fps() const noexcept
{
  return this->_fps;
}

[[nodiscard]] auto w_gametime::get_fixed_time_step() const noexcept
{
  return this->_fixed_time_step;
}

void w_gametime::set_fixed_time_step(bool p_value) noexcept
{
  this->_fixed_time_step = p_value;
}

void w_gametime::set_target_elapsed_secs(double p_value) noexcept
{
  this->_target_elapsed_secs = p_value;
}
