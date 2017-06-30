/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_game_time.h
	Description		 : Manage game time
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GAME_TIME_H__
#define __W_GAME_TIME_H__

#if defined(__WIN32) || defined(__UWP)
#include "Winbase.h"
#else
#include "w_std.h"
#endif

#include <string>
#include <typeinfo>
#include <ctime>

#include <exception>
#include "w_logger.h"


#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace system
	{
		class w_game_time
		{
		public:
			w_game_time() :
				_elapsed_ticks(0),
				_total_ticks(0),
				_left_over_ticks(0),
				_frame_count(0),
				_fps(0),
				_frames_this_second(0),
				_seconds_counter(0),
				_fixed_time_step(false),
				_target_elapsed_ticks(TICKS_PER_SECOND / 60)
			{
				this->_name = "game_time";

#if defined(__WIN32) || defined(__UWP)
				//Get frequency
				if (!QueryPerformanceFrequency(&this->_frequency))
				{
					V(S_FALSE, "query performance frequency (on constructor)", this->_name, 3, true, false);
				}

				//Get performance
				if (!QueryPerformanceCounter(&this->_last_time))
				{
					V(S_FALSE, "query performance frequency (on constructor)", this->_name, 3, true, false);
				}

				// Initialize max delta to 1/10 of a second.
				this->_max_delta = this->_frequency.QuadPart / 10;
#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

				reset_elapsed_time();
				this->_max_delta = 313918;
#endif
			}

#pragma region Getters

			// Get elapsed time since the previous Update call.
			UINT64 get_elapsed_ticks() const									{ return this->_elapsed_ticks; }
			double get_elapsed_seconds() const									{ return ticks_to_seconds(this->_elapsed_ticks); }

			// Get total time since the start of the program.
			UINT64 get_total_ticks() const										{ return this->_total_ticks; }
			double get_total_seconds() const									{ return ticks_to_seconds(this->_total_ticks); }

			// Get total number of updates since start of the program.
			UINT32 get_frame_count() const										{ return this->_frame_count; }

			// Get the current framerate.
			UINT32 get_frames_per_second() const								{ return this->_fps; }

			bool get_fixed_time_step() const										{ return this->_fixed_time_step; }

#pragma endregion

#pragma region Setters

			// Set whether to use fixed or variable timestep mode.
			void set_fixed_time_step(bool pValue)								{ this->_fixed_time_step = pValue; }

			// Set how often to call Update when in fixed timestep mode.
			void set_target_elapsed_ticks(UINT64 pValue)						{ this->_target_elapsed_ticks = pValue; }
			void set_target_elapsed_seconds(double pValue)						{ this->_target_elapsed_ticks = seconds_to_ticks(pValue); }

#pragma endregion

			// Integer format represents time using 10,000,000 ticks per second.
			static const UINT64 TICKS_PER_SECOND = 10000000;

			static double ticks_to_seconds(UINT64 pTicks)						{ return static_cast<double>(pTicks) / TICKS_PER_SECOND; }
			static UINT64 seconds_to_ticks(double pSeconds)						{ return static_cast<UINT64>(pSeconds * TICKS_PER_SECOND); }

			void reset()
			{
				this->_last_time = get_time();

				this->_left_over_ticks = 0;
				this->_fps = 0;
				this->_frames_this_second = 0;
				this->_seconds_counter = 0;
			}

			// Update timer state, calling the specified Update function the appropriate number of times.
			template<typename TUpdate>
			void tick(const TUpdate& pUpdate)
			{
				// Query the current time.
				auto _current_time = get_time();

#if defined(__WIN32) || defined(__UWP)
				auto _time_delta = _current_time.QuadPart - this->_last_time.QuadPart;
#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
				auto _time_delta = _current_time - this->_last_time;
#endif

				this->_last_time = _current_time;
				this->_seconds_counter += _time_delta;

				// Clamp excessively large time deltas (e.g. after paused in the debugger).
				if (_time_delta > this->_max_delta)
				{
					_time_delta = this->_max_delta;
				}

				// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
				_time_delta *= TICKS_PER_SECOND;
#if defined(__WIN32) || defined(__UWP)
				if (this->_frequency.QuadPart == 0)
				{
					logger.write(L"Division by Zero for frequency of CPU");
				}
				else
				{
					_time_delta /= this->_frequency.QuadPart;
				}
#endif

				auto _last_frame_count = this->_frame_count;

				if (this->_fixed_time_step)
				{
					/*
						If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
						the clock to exactly match the target value. This prevents tiny and irrelevant errors
						from accumulating over time. Without this clamping, a game that requested a 60 fps
						fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
						accumulate enough tiny errors that it would drop a frame. It is better to just round
						small deviations down to zero to leave things running smoothly.
					*/

					if (std::abs(static_cast<INT64>(_time_delta - this->_target_elapsed_ticks)) < TICKS_PER_SECOND / 4000)
					{
						_time_delta = this->_target_elapsed_ticks;
					}

					this->_left_over_ticks += _time_delta;

					while (this->_left_over_ticks >= this->_target_elapsed_ticks)
					{
						this->_elapsed_ticks = this->_target_elapsed_ticks;
						this->_total_ticks += this->_target_elapsed_ticks;
						this->_left_over_ticks -= this->_target_elapsed_ticks;
						this->_frame_count++;

						pUpdate();
					}
				}
				else
				{
					// Variable timestep update logic.
					this->_elapsed_ticks = _time_delta;
					this->_total_ticks += _time_delta;
					this->_left_over_ticks = 0;
					this->_frame_count++;

					pUpdate();
				}

				// Track the current framerate.
				this->_frames_this_second += (this->_frame_count - _last_frame_count);

#if defined(__WIN32) || defined(__UWP)
				const auto _one_sec = static_cast<UINT64>(this->_frequency.QuadPart);
#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
				const auto _one_sec = 1;
#endif

				if (this->_seconds_counter >= _one_sec)
				{
					this->_fps = this->_frames_this_second;

					this->_frames_this_second = 0;

#if defined(__WIN32) || defined(__UWP)
					if (_one_sec == 0)
					{
						logger.write(L"Division by Zero for frequency of CPU");
					}
					else
					{
						this->_seconds_counter %= this->_frequency.QuadPart;
					}

#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
					this->_seconds_counter %= _one_sec;
#endif
				}
			}

		private:
			std::string _name;


#if defined(__WIN32) || defined(__UWP)
			//get current time in second
			LARGE_INTEGER get_time()
			{
				LARGE_INTEGER _time;
				if (!QueryPerformanceCounter(&_time))
				{
					V(S_FALSE, "query performance on get_time method pf w_game_time", this->_name, 3, true, false);
				}
				return _time;
			}
#else
			INT64 get_time()
			{
				struct timespec _now;
				clock_gettime(CLOCK_MONOTONIC, &_now);
				auto _nano_sconds = static_cast<INT64>(_now.tv_sec * 1000000000LL + _now.tv_nsec);
				return  _nano_sconds / 1000000000LL;
			}
#endif


#if defined(__WIN32) || defined(__UWP)
			// Source timing data uses Query Performance Counter units.
			LARGE_INTEGER _frequency;
			LARGE_INTEGER _last_time;

#else
			INT64	_last_time;
#endif
			UINT64 _max_delta;

			// Derived timing data uses a canonical tick format.
			UINT64 _elapsed_ticks;
			UINT64 _total_ticks;
			UINT64 _left_over_ticks;

			// Members for tracking the framerate.
			UINT32 _frame_count;
			UINT32 _fps;
			UINT32 _frames_this_second;
			UINT64 _seconds_counter;

			// Members for configuring fixed timestep mode.
			bool _fixed_time_step;
			UINT64 _target_elapsed_ticks;
		};
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_GAME_TIME_H__
