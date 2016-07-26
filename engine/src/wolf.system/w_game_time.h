/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_game_time.h
	Description		 : Manage game time
	Comment          :
*/

#ifndef __W_GAME_TIME_H__
#define __W_GAME_TIME_H__

#include <string>
#include <typeinfo>
#include <ctime>
#include "Winbase.h"
#include <exception>
#include "w_logger.h"

namespace wolf
{
	namespace system
	{
		class w_game_time
		{
		public:
			w_game_time() :
				_elapsedTicks(0),
				_totalTicks(0),
				_leftOverTicks(0),
				_frameCount(0),
				_fps(0),
				_framesThisSecond(0),
				_qpcSecondCounter(0),
				_fixedTimeStep(false),
				_targetElapsedTicks(TICKS_PER_SECOND / 60)
			{
				this->_name = "game_time";

				//Get frequency
				if (!QueryPerformanceFrequency(&this->_qpFrequency))
				{
					V(S_FALSE, "query performance frequency (on constructor)", this->_name, 3, true, false);
				}

				//Get performance
				if (!QueryPerformanceCounter(&this->_qpLastTime))
				{
					V(S_FALSE, "query performance frequency (on constructor)", this->_name, 3, true, false);
				}

				// Initialize max delta to 1/10 of a second.
				this->_qpMaxDelta = this->_qpFrequency.QuadPart / 10;
			}

#pragma region Getters

			// Get elapsed time since the previous Update call. 
			UINT64 get_elapsed_ticks() const									{ return this->_elapsedTicks; }
			double get_elapsed_seconds() const									{ return ticks_to_seconds(this->_elapsedTicks); }

			// Get total time since the start of the program.
			UINT64 get_total_ticks() const										{ return this->_totalTicks; }
			double get_total_seconds() const									{ return ticks_to_seconds(this->_totalTicks); }

			// Get total number of updates since start of the program.
			UINT32 get_frame_count() const										{ return this->_frameCount; }

			// Get the current framerate.
			UINT32 get_frames_per_second() const								{ return this->_fps; }

			bool get_fixed_timeStep() const										{ return this->_fixedTimeStep; }

#pragma endregion

#pragma region Setters

			// Set whether to use fixed or variable timestep mode.
			void set_fixed_time_step(bool pValue)								{ this->_fixedTimeStep = pValue; }

			// Set how often to call Update when in fixed timestep mode.
			void set_target_elapsed_ticks(UINT64 pValue)						{ this->_targetElapsedTicks = pValue; }
			void set_target_elapsed_seconds(double pValue)						{ this->_targetElapsedTicks = seconds_to_ticks(pValue); }

#pragma endregion

			// Integer format represents time using 10,000,000 ticks per second.
			static const UINT64 TICKS_PER_SECOND = 10000000;

			static double ticks_to_seconds(UINT64 pTicks)						{ return static_cast<double>(pTicks) / TICKS_PER_SECOND; }
			static UINT64 seconds_to_ticks(double pSeconds)						{ return static_cast<UINT64>(pSeconds * TICKS_PER_SECOND); }

			void reset_elapsed_time()
			{
				if (!QueryPerformanceCounter(&this->_qpLastTime))
				{
					V(S_FALSE, "query performance frequency (reset elapsed time)", this->_name, 3, true, false);
				}

				this->_leftOverTicks = 0;
				this->_fps = 0;
				this->_framesThisSecond = 0;
				this->_qpcSecondCounter = 0;
			}

			// Update timer state, calling the specified Update function the appropriate number of times.
			template<typename TUpdate>
			void tick(const TUpdate& pUpdate)
			{
				// Query the current time.
				LARGE_INTEGER _currentTime;

				if (!QueryPerformanceCounter(&_currentTime))
				{
					V(S_FALSE, "query performance frequency (Tick)", this->_name, 3, true, false);
				}

				UINT64 _timeDelta = _currentTime.QuadPart - this->_qpLastTime.QuadPart;

				this->_qpLastTime = _currentTime;
				this->_qpcSecondCounter += _timeDelta;

				// Clamp excessively large time deltas (e.g. after paused in the debugger).
				if (_timeDelta > this->_qpMaxDelta)
				{
					_timeDelta = this->_qpMaxDelta;
				}

				// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
				_timeDelta *= TICKS_PER_SECOND;
				if (this->_qpFrequency.QuadPart == 0)
				{
					logger.write(L"Division by Zero for frequency of CPU");
				}
				else
				{
					_timeDelta /= this->_qpFrequency.QuadPart;
				}

				auto _lastFrameCount = this->_frameCount;

				if (this->_fixedTimeStep)
				{
					/*
						If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
						the clock to exactly match the target value. This prevents tiny and irrelevant errors
						from accumulating over time. Without this clamping, a game that requested a 60 fps
						fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
						accumulate enough tiny errors that it would drop a frame. It is better to just round
						small deviations down to zero to leave things running smoothly.
					*/

					if (abs(static_cast<INT64>(_timeDelta - this->_targetElapsedTicks)) < TICKS_PER_SECOND / 4000)
					{
						_timeDelta = this->_targetElapsedTicks;
					}

					this->_leftOverTicks += _timeDelta;

					while (this->_leftOverTicks >= this->_targetElapsedTicks)
					{
						this->_elapsedTicks = this->_targetElapsedTicks;
						this->_totalTicks += this->_targetElapsedTicks;
						this->_leftOverTicks -= this->_targetElapsedTicks;
						this->_frameCount++;

						pUpdate();
					}
				}
				else
				{
					// Variable timestep update logic.
					this->_elapsedTicks = _timeDelta;
					this->_totalTicks += _timeDelta;
					this->_leftOverTicks = 0;
					this->_frameCount++;

					pUpdate();
				}

				// Track the current framerate.
				if (this->_frameCount != _lastFrameCount)
				{
					this->_framesThisSecond++;
				}

				if (this->_qpcSecondCounter >= static_cast<UINT64>(this->_qpFrequency.QuadPart))
				{
					this->_fps = this->_framesThisSecond;
					
					this->_framesThisSecond = 0;
					if (this->_qpFrequency.QuadPart == 0)
					{
						logger.write(L"Division by Zero for frequency of CPU");
					}
					else
					{
						this->_qpcSecondCounter %= this->_qpFrequency.QuadPart;
					}
				}
			}

		private:
			std::string _name;

			// Source timing data uses Query Performance Counter units.
			LARGE_INTEGER _qpFrequency;
			LARGE_INTEGER _qpLastTime;
			UINT64 _qpMaxDelta;

			// Derived timing data uses a canonical tick format.
			UINT64 _elapsedTicks;
			UINT64 _totalTicks;
			UINT64 _leftOverTicks;

			// Members for tracking the framerate.
			UINT32 _frameCount;
			UINT32 _fps;
			UINT32 _framesThisSecond;
			UINT64 _qpcSecondCounter;

			// Members for configuring fixed timestep mode.
			bool _fixedTimeStep;
			UINT64 _targetElapsedTicks;
		};
	}
}

#endif