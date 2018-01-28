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

#include "python_exporter/w_boost_python_helper.h"

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
				_target_elapsed_ticks((uint64_t)(TICKS_PER_SECOND / 60))
			{
				this->_name = "game_time";

#if defined(__WIN32) || defined(__UWP)
				//Get frequency
				if (!QueryPerformanceFrequency(&this->_frequency))
				{
					V(S_FALSE, "query performance frequency (on constructor)", this->_name, 3, true);
				}

				//Get performance
				if (!QueryPerformanceCounter(&this->_last_time))
				{
					V(S_FALSE, "query performance frequency (on constructor)", this->_name, 3, true);
				}

				// Initialize max delta to 1/10 of a second.
				this->_max_delta = (double)(this->_frequency.QuadPart / 10);
#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

				reset();
				this->_max_delta = 313918.0;
#endif
			}

#pragma region Getters

			// Get elapsed time since the previous Update call.
			uint64_t get_elapsed_ticks() const									{ return this->_elapsed_ticks; }
			double get_elapsed_seconds() const                                  { return ticks_to_seconds(this->_elapsed_ticks); }

			// Get total time since the start of the program.
			uint64_t get_total_ticks() const									{ return this->_total_ticks; }
			double get_total_seconds() const									{ return ticks_to_seconds(this->_total_ticks); }

			// Get total number of updates since start of the program.
			uint32_t get_frame_count() const									{ return this->_frame_count; }

			// Get the current framerate.
			uint32_t get_frames_per_second() const								{ return this->_fps; }

			bool get_fixed_time_step() const									{ return this->_fixed_time_step; }

#pragma endregion

#pragma region Setters

			// Set whether to use fixed or variable timestep mode.
			void set_fixed_time_step(bool pValue)								{ this->_fixed_time_step = pValue; }

			// Set how often to call Update when in fixed timestep mode.
			void set_target_elapsed_ticks(uint64_t pValue)						{ this->_target_elapsed_ticks = pValue; }
			void set_target_elapsed_seconds(double pValue)						{ this->_target_elapsed_ticks = seconds_to_ticks(pValue); }

#pragma endregion

			// Integer format represents time using 10,000,000 ticks per second.
			static constexpr double TICKS_PER_SECOND = 10000000;

			static double ticks_to_seconds(uint64_t pTicks)						{ return (double)pTicks / TICKS_PER_SECOND; }
			static uint64_t seconds_to_ticks(double pSeconds)					{ return static_cast<uint64_t>(pSeconds * TICKS_PER_SECOND); }

			void reset()
			{
				this->_last_time = _get_time();

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
				auto _current_time = _get_time();

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

					if (std::abs(static_cast<int64_t>(_time_delta - this->_target_elapsed_ticks)) < TICKS_PER_SECOND / 4000)
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
					this->_elapsed_ticks = (uint64_t)_time_delta;
					this->_total_ticks += (uint64_t)_time_delta;
					this->_left_over_ticks = 0;
					this->_frame_count++;

					pUpdate();
				}

				// Track the current framerate.
				this->_frames_this_second += (this->_frame_count - _last_frame_count);

#if defined(__WIN32) || defined(__UWP)
				const auto _one_sec = static_cast<uint64_t>(this->_frequency.QuadPart);
#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
				const auto _one_sec = 1.0;
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
						this->_seconds_counter = (uint64_t)this->_seconds_counter % this->_frequency.QuadPart;
					}

#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
					this->_seconds_counter = ((uint64_t)this->_seconds_counter % (uint64_t)_one_sec);
#endif
				}
			}

#ifdef __PYTHON__
            double py_TICKS_PER_SECOND = TICKS_PER_SECOND;
            double py_ticks_to_seconds(uint64_t pTicks) { return  ticks_to_seconds(pTicks); }
            uint64_t py_seconds_to_ticks(double pSeconds) { return ticks_to_seconds(pSeconds); }
            static void py_tick(w_game_time& pSelf, boost::python::object pObject) { return pSelf.tick(pObject); }
#endif

		private:
			std::string _name;


#if defined(__WIN32) || defined(__UWP)
			//get current time in second
			LARGE_INTEGER _get_time()
			{
				LARGE_INTEGER _time;
				if (!QueryPerformanceCounter(&_time))
				{
					V(S_FALSE, "query performance on _get_time method of w_game_time", this->_name, 3, true);
				}
				return _time;
			}
#else
			double _get_time()
			{
				struct timespec _now;
				clock_gettime(CLOCK_MONOTONIC, &_now);
				auto _sconds = (double)_now.tv_sec  + (double)_now.tv_nsec / 1000000000.0;
				return  _sconds;
			}
#endif


#if defined(__WIN32) || defined(__UWP)
			// Source timing data uses Query Performance Counter units.
			LARGE_INTEGER _frequency;
			LARGE_INTEGER _last_time;

#else
			double	_last_time;
#endif
			double _max_delta;

			// Derived timing data uses a canonical tick format.
			uint64_t _elapsed_ticks;
			uint64_t _total_ticks;
			uint64_t _left_over_ticks;

			// Members for tracking the framerate.
			uint32_t _frame_count;
			uint32_t _fps;
			uint32_t _frames_this_second;
			double _seconds_counter;

			// Members for configuring fixed timestep mode.
			bool _fixed_time_step;
			uint64_t _target_elapsed_ticks;
		};
	}
}

#ifdef __PYTHON__
#include "python_exporter/w_game_time_py.h"
#endif


#endif //__W_GAME_TIME_H__
