/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_timer.h
	Description		 : A timer based on QueryPerformanceCounter
	Comment          :
*/

#pragma once

#include "w_game_time.h"

namespace wolf::system
{
	class w_timer
	{
		friend class w_timer_callback;
	public:
		// Constructor of Timer
		w_timer()
		{
			_timer.set_fixed_time_step(false);
		}

		~w_timer()
		{
		}

		void start()
		{
			_timer.tick([]() { /*NOP*/ });
		}

		void stop()
		{
			_timer.tick([]() { /*NOP*/ });
		}

		void reset()
		{
			_timer.reset();
			_timer.tick([]() { /*NOP*/ });
		}

		//Get total time in seconds
		double get_seconds()
		{
			_timer.tick([]() { /*NOP*/ });
			return _timer.get_total_seconds();
		};

		//Get total time in milliseconds
		double get_milliseconds()
		{
			_timer.tick([]() { /*NOP*/ });
			return _timer.get_total_seconds() * 1000;
		};

	private:
		w_game_time		_timer;
	};
}

#include "python_exporter/py_timer.h"

