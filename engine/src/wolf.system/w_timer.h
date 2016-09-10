/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_timer.h
	Description		 : A timer based on QueryPerformanceCounter
	Comment          :
*/

#ifndef __W_TIMER_H__
#define __W_TIMER_H__

#include <windows.h>

namespace wolf
{
	namespace system
	{
		class w_timer sealed
		{
			friend class w_timer_callback;
		public:
			// Constructor of Timer
			w_timer()
			{
				if (!QueryPerformanceFrequency(&this->_frequency))
				{
					throw "QueryPerformanceFrequency failed in GameTime.h";
				}
			}

			~w_timer()
			{
			}

			void start()
			{
				update();
				this->_startTime = this->_currentTime;
				this->_totalTime = 0.0f;
				this->_deltaTime = 1.0f / 60.0f;
			}

			void stop()
			{
				update();
			}

			//Get total time in seconds
			double getTotalSeconds()
			{
				return this->_totalTime;
			};

			//Get total time in milliseconds
			double getTotalMilliSeconds()
			{
				return this->_totalTime * 1000;
			};

			//Get elpased time in seconds
			double getElapsedSeconds()
			{
				return this->_deltaTime;
			};

			//Get elpased time in milliseconds
			double getElapsedMilliSeconds()
			{
				return this->_deltaTime * 1000;
			};

		private:
			LARGE_INTEGER _frequency, _currentTime, _startTime, _lastTime;
			double _totalTime, _deltaTime;

			void update()
			{
				if (!QueryPerformanceCounter(&this->_currentTime))
				{
					throw "QueryPerformanceCounter failed in GameTime.h";
				}

				this->_totalTime = static_cast<float>(
					static_cast<double>(this->_currentTime.QuadPart - this->_startTime.QuadPart) / static_cast<double>(this->_frequency.QuadPart));

				if (this->_lastTime.QuadPart == this->_startTime.QuadPart)
				{
					// If the timer was just reset, report a time delta equivalent to 60Hz frame time.
					this->_deltaTime = 1.0f / 60.0f;
				}
				else
				{
					this->_deltaTime = static_cast<float>(
						static_cast<double>(this->_currentTime.QuadPart - this->_lastTime.QuadPart) /
						static_cast<double>(this->_frequency.QuadPart));
				}

				this->_lastTime = this->_currentTime;
			}
		};
	}
}

#endif