/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_GameTime.h
	Description		 : Manage game time
	Comment          :
*/

#pragma once

#include <string>
#include <typeinfo>
#include <ctime>
#include "Winbase.h"
#include <exception>
#include "W_Logger.h"

namespace Wolf
{
	namespace System
	{
		class W_GameTime
		{
		public:
			W_GameTime() :
				elapsedTicks(0),
				totalTicks(0),
				leftOverTicks(0),
				frameCount(0),
				fps(0),
				framesThisSecond(0),
				qpcSecondCounter(0),
				fixedTimeStep(false),
				targetElapsedTicks(TicksPerSecond / 60)
			{
				this->name = "GameTime";

				//Get frequency
				if (!QueryPerformanceFrequency(&this->qpFrequency))
				{
					OnFailed(S_FALSE, "query performance frequency (on constructor)", this->name, true);
				}

				//Get performance
				if (!QueryPerformanceCounter(&this->qpLastTime))
				{
					OnFailed(S_FALSE, "query performance frequency (on constructor)", this->name, true);
				}

				// Initialize max delta to 1/10 of a second.
				this->qpMaxDelta = this->qpFrequency.QuadPart / 10;
			}

#pragma region Getters

			// Get elapsed time since the previous Update call. 
			UINT64 GetElapsedTicks() const										{ return this->elapsedTicks; }
			double GetElapsedSeconds() const									{ return TicksToSeconds(this->elapsedTicks); }

			// Get total time since the start of the program.
			UINT64 GetTotalTicks() const										{ return this->totalTicks; }
			double GetTotalSeconds() const										{ return TicksToSeconds(this->totalTicks); }

			// Get total number of updates since start of the program.
			UINT32 GetFrameCount() const										{ return this->frameCount; }

			// Get the current framerate.
			UINT32 GetFramesPerSecond() const									{ return this->fps; }

			bool GetFixedTimeStep() const										{ return this->fixedTimeStep; }

#pragma endregion

#pragma region Setters

			// Set whether to use fixed or variable timestep mode.
			void SetFixedTimeStep(bool value)									{ this->fixedTimeStep = value; }

			// Set how often to call Update when in fixed timestep mode.
			void SetTargetElapsedTicks(UINT64 value)							{ this->targetElapsedTicks = value; }
			void SetTargetElapsedSeconds(double value)							{ this->targetElapsedTicks = SecondsToTicks(value); }

#pragma endregion

			// Integer format represents time using 10,000,000 ticks per second.
			static const UINT64 TicksPerSecond = 10000000;

			static double TicksToSeconds(UINT64 ticks)			{ return static_cast<double>(ticks) / TicksPerSecond; }
			static UINT64 SecondsToTicks(double seconds)		{ return static_cast<UINT64>(seconds * TicksPerSecond); }

			void ResetElapsedTime()
			{
				if (!QueryPerformanceCounter(&this->qpLastTime))
				{
					OnFailed(S_FALSE, "query performance frequency (reset elapsed time)", this->name, true);
				}

				this->leftOverTicks = 0;
				this->fps = 0;
				this->framesThisSecond = 0;
				this->qpcSecondCounter = 0;
			}

			// Update timer state, calling the specified Update function the appropriate number of times.
			template<typename TUpdate>
			void Tick(const TUpdate& update)
			{
				// Query the current time.
				LARGE_INTEGER currentTime;

				if (!QueryPerformanceCounter(&currentTime))
				{
					OnFailed(S_FALSE, "query performance frequency (Tick)", this->name, true);
				}

				UINT64 timeDelta = currentTime.QuadPart - this->qpLastTime.QuadPart;

				this->qpLastTime = currentTime;
				this->qpcSecondCounter += timeDelta;

				// Clamp excessively large time deltas (e.g. after paused in the debugger).
				if (timeDelta > this->qpMaxDelta)
				{
					timeDelta = this->qpMaxDelta;
				}

				// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
				timeDelta *= TicksPerSecond;
				if (this->qpFrequency.QuadPart == 0)
				{
					Logger.Write("Division by Zero for frequency of CPU");
				}
				else
				{
					timeDelta /= this->qpFrequency.QuadPart;
				}

				auto lastFrameCount = this->frameCount;

				if (this->fixedTimeStep)
				{
					/*
						If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
						the clock to exactly match the target value. This prevents tiny and irrelevant errors
						from accumulating over time. Without this clamping, a game that requested a 60 fps
						fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
						accumulate enough tiny errors that it would drop a frame. It is better to just round
						small deviations down to zero to leave things running smoothly.
					*/

					if (abs(static_cast<INT64>(timeDelta - this->targetElapsedTicks)) < TicksPerSecond / 4000)
					{
						timeDelta = this->targetElapsedTicks;
					}

					this->leftOverTicks += timeDelta;

					while (this->leftOverTicks >= this->targetElapsedTicks)
					{
						this->elapsedTicks = this->targetElapsedTicks;
						this->totalTicks += this->targetElapsedTicks;
						this->leftOverTicks -= this->targetElapsedTicks;
						this->frameCount++;

						update();
					}
				}
				else
				{
					// Variable timestep update logic.
					this->elapsedTicks = timeDelta;
					this->totalTicks += timeDelta;
					this->leftOverTicks = 0;
					this->frameCount++;

					update();
				}

				// Track the current framerate.
				if (this->frameCount != lastFrameCount)
				{
					this->framesThisSecond++;
				}

				if (this->qpcSecondCounter >= static_cast<UINT64>(this->qpFrequency.QuadPart))
				{
					this->fps = this->framesThisSecond;
					this->framesThisSecond = 0;
					if (this->qpFrequency.QuadPart == 0)
					{
						Logger.Write("Division by Zero for frequency of CPU");
					}
					else
					{
						this->qpcSecondCounter %= this->qpFrequency.QuadPart;
					}
				}
			}

		private:
			std::string name;

			// Source timing data uses Query Performance Counter units.
			LARGE_INTEGER qpFrequency;
			LARGE_INTEGER qpLastTime;
			UINT64 qpMaxDelta;

			// Derived timing data uses a canonical tick format.
			UINT64 elapsedTicks;
			UINT64 totalTicks;
			UINT64 leftOverTicks;

			// Members for tracking the framerate.
			UINT32 frameCount;
			UINT32 fps;
			UINT32 framesThisSecond;
			UINT64 qpcSecondCounter;

			// Members for configuring fixed timestep mode.
			bool fixedTimeStep;
			UINT64 targetElapsedTicks;
		};
	}
}