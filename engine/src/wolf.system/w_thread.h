/*
    Project             : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source             : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website             : https://WolfEngine.App
    Name             : w_thread.h
    Description         : A cross platform thread class
    Comment          :
*/

#pragma once

#if  defined(__WIN32) || defined(__UWP)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#include <thread>
#include <queue>

#include "w_system_export.h"
#include <functional>

#if defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
#include "w_std.h"
#endif

namespace wolf::system
{
	class w_thread_pimp
	{
		friend class w_thread;

#ifdef __WIN32
		friend unsigned int __stdcall THREAD_WORK(_In_ void* pParameter);
#endif
	private:
		w_thread_pimp() : _is_released(false)
		{
#ifdef __WIN32
			InitializeCriticalSection(&this->_critical_section);
			InitializeConditionVariable(&this->_condition_var);
			this->_thread_handle = (HANDLE)_beginthreadex(0, 0, THREAD_WORK, this, 0, 0);
#else
			this->_thread_handle = std::thread(&w_thread_pimp::action, this);
#endif
		}
		~w_thread_pimp()
		{
			release();
		}

		void action();

		void add_job(_In_ const std::function<void()>& pJob)
		{
#ifdef __WIN32
			EnterCriticalSection(&this->_critical_section);
#else
			std::lock_guard<std::mutex> _lock(this->_critical_section);
#endif

			this->_job_queue.push(std::move(pJob));

			//signal for action function
#ifdef __WIN32
			WakeConditionVariable(&this->_condition_var);
			LeaveCriticalSection(&this->_critical_section);
#else
			this->_condition_var.notify_one();
#endif
		}

		void notify_one()
		{
#ifdef __WIN32
			WakeConditionVariable(&this->_condition_var);
#else
			this->_condition_var.notify_one();
#endif
		}

		void wait_until_complete()
		{
#ifdef __WIN32
			EnterCriticalSection(&this->_critical_section);
			while (!this->_job_queue.empty())
			{
				SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, INFINITE);
			}
			LeaveCriticalSection(&this->_critical_section);
#else
			std::unique_lock<std::mutex> _lock(this->_critical_section);
			this->_condition_var.wait(_lock, [this]()
				{
					return this->_job_queue.empty();
				});
#endif
		}

		void wait()
		{
#ifdef __WIN32
			EnterCriticalSection(&this->_critical_section);
			SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, INFINITE);
			LeaveCriticalSection(&this->_critical_section);
#else
			std::unique_lock<std::mutex> _lock(this->_critical_section);
			this->_condition_var.wait(_lock);
#endif
		}

		template <typename Predicate>
		void wait(Predicate pred)
		{
#ifdef __WIN32
			EnterCriticalSection(&this->_critical_section);
			while (!pred())
			{
				SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, INFINITE);
			}
			LeaveCriticalSection(&this->_critical_section);
#else
			std::unique_lock<std::mutex> _lock(this->_critical_section);
			this->_condition_var.wait(_lock, pred);
#endif
		}

				template <typename Rep, typename Period>
				int wait_for(const std::chrono::duration<Rep, Period>& rel_time)
				{
		#ifdef __WIN32
					EnterCriticalSection(&this->_critical_section);
					bool _result = SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, rel_time.count());
					LeaveCriticalSection(&this->_critical_section);
		
					if (!_result)
					{
						return W_TIMEOUT;
					}
					else
					{
						return W_NOTIFIED;
					}
		#else
					std::unique_lock<std::mutex> _lock(this->_critical_section);
					if (this->_condition_var.wait_for(_lock, rel_time) == std::cv_status::timeout)
					{
						return W_TIMEOUT;
					}
					else
					{
						return W_NOTIFIED;
					}
		#endif
				}

				template <typename Rep, typename Period, typename Predicate>
				int wait_for(const std::chrono::duration<Rep, Period>& rel_time, Predicate pred)
				{
#ifdef __WIN32
					EnterCriticalSection(&this->_critical_section);
					bool _result = 0;

					while (!pred())
					{
						_result = SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, rel_time.count());

						if (!_result)
							break;
					}
					LeaveCriticalSection(&this->_critical_section);

					if (!_result)
					{
						return W_TIMEOUT;
					}
					else if (pred())
					{
						return W_CONDITION_PASSED;
					}
					else
					{
						return W_CONDITION_FAILED;
					}
#else
					std::unique_lock<std::mutex> _lock(this->_critical_section);
					if (this->_condition_var.wait_for(_lock, rel_time, pred) == std::cv_status::timeout)
					{
						return W_TIMEOUT;
					}
					else if (pred())
					{
						return W_CONDITION_PASSED;
					}
					else
					{
						return W_CONDITION_FAILED;
					}
#endif
				}

				template <typename Clock, typename Duration>
				int wait_until(const std::chrono::time_point<Clock, Duration>& abs_time)
				{
#ifdef __WIN32
					bool _result = 0;
					std::chrono::steady_clock::time_point _now = std::chrono::steady_clock::now();
					auto _time = std::chrono::duration_cast<std::chrono::milliseconds>(_now - abs_time).count();

					if (_time > 0)
					{
						EnterCriticalSection(&this->_critical_section);
						_result = SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, _time);
						LeaveCriticalSection(&this->_critical_section);
					}

					if (!_result)
					{
						return W_TIMEOUT;
					}
					else
					{
						return W_NOTIFIED;
					}
#else
					std::unique_lock<std::mutex> _lock(this->_critical_section);
					if (this->_condition_var.wait_until(_lock, abs_time) == std::cv_status::timeout)
					{
						return W_TIMEOUT;
					}
					else
					{
						return W_NOTIFIED;
					}
#endif
				}

				template <typename Clock, typename Duration, typename Predicate>
				int wait_until(const std::chrono::time_point<Clock, Duration>& abs_time,
					Predicate pred)
				{
#ifdef __WIN32
					EnterCriticalSection(&this->_critical_section);
					bool _result = 0;

					while (!pred())
					{
						std::chrono::steady_clock::time_point _now = std::chrono::steady_clock::now();
						auto _time = std::chrono::duration_cast<std::chrono::milliseconds>(_now - abs_time).count();

						if (_time > 0)
							_result = SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, _time);
						else break;

						if (!_result)
							break;
					}
					LeaveCriticalSection(&this->_critical_section);

					if (!_result)
					{
						return W_TIMEOUT;
					}
					else if (pred())
					{
						return W_CONDITION_PASSED;
					}
					else
					{
						return W_CONDITION_FAILED;
					}
#else
					std::unique_lock<std::mutex> _lock(this->_critical_section);
					if (this->_condition_var.wait_until(_lock, abs_time, pred) == std::cv_status::timeout)
					{
						return W_TIMEOUT;
					}
					else if (pred())
					{
						return W_CONDITION_PASSED;
					}
					else
					{
						return W_CONDITION_FAILED;
					}
#endif
				}

		ULONG release()
		{
			if (this->_is_released) return 1;

#ifdef __WIN32
			EnterCriticalSection(&this->_critical_section);
			this->_is_released = true;

			auto _jobs_count = this->_job_queue.size();
			if (_jobs_count)
			{
				while (_jobs_count)
				{
					this->_job_queue.pop();
					_jobs_count = this->_job_queue.size();
				}
			}
			else
			{
				//Signal to action to wake up and break from loop
				WakeConditionVariable(&this->_condition_var);
			}

			LeaveCriticalSection(&this->_critical_section);
#else
			if (this->_thread_handle.joinable())
			{
				wait_until_complete();
				this->_critical_section.lock();
				this->_is_released = true;
				this->_condition_var.notify_one();
				this->_critical_section.unlock();
				this->_thread_handle.join();

				//  logger.write("thread with id: {}  destroyed", _thread_id);
			}
#endif
			return 0;
		}

#ifdef __WIN32
		void destroy()
		{
			//close thread
			CloseHandle(this->_thread_handle);
			DeleteCriticalSection(&this->_critical_section);
			delete this;
		}
#endif

		DWORD get_thread_id()
		{
			return this->_thread_id;
		}

	private:
#if defined(__WIN32) || defined(__UWP)
		HANDLE                                  _thread_handle;
		CRITICAL_SECTION                        _critical_section;
		CONDITION_VARIABLE                      _condition_var;
#else
		std::thread                             _thread_handle;
		std::mutex                              _critical_section;
		std::condition_variable                 _condition_var;
#endif

		DWORD                                   _thread_id;
		std::queue<std::function<void()>>       _job_queue;
		bool                                    _is_released;
	};

    class w_thread
    {
    public:
        WSYS_EXP w_thread();
        WSYS_EXP ~w_thread();

        WSYS_EXP void add_job(_In_ const std::function<void()>& pJob);
        WSYS_EXP void notify_one();
        WSYS_EXP void wait_until_complete();
        WSYS_EXP void wait();
        WSYS_EXP void release();
        
        template <typename Rep, typename Period>
		int wait_for(const std::chrono::duration<Rep, Period>& rel_time)
		{
				if (!this->_pimp) return W_INVALID_THREAD;
				return this->_pimp->wait_for<Rep, Period>(rel_time);
		}
        
		template <typename Rep, typename Period, typename Predicate>
		int wait_for(const std::chrono::duration<Rep, Period>& rel_time,
			Predicate pred)
		{
			if (!this->_pimp) return W_INVALID_THREAD;
			return this->_pimp->wait_for<Rep, Period, Predicate>(rel_time, pred);
		}

		template <typename Clock, typename Duration>
		int wait_until(const std::chrono::time_point<Clock, Duration>& abs_time)
		{
			if (!this->_pimp) return W_INVALID_THREAD;
			return this->_pimp->wait_until<Clock, Duration>(abs_time);
		}

		template <typename Clock, typename Duration, typename Predicate>
		int wait_until(const std::chrono::time_point<Clock, Duration>& abs_time,
			Predicate pred)
		{
			if (!this->_pimp) return W_INVALID_THREAD;
			return this->_pimp->wait_until<Clock, Duration, Predicate>(abs_time, pred);
		}

		template <typename Predicate>
		void wait(Predicate pred)
		{
			if (!this->_pimp) return;
			this->_pimp->wait<Predicate>(pred);
		}

        //hardware_thread_contexts usually equal to number of CPU cores
        WSYS_EXP static unsigned int get_number_of_hardware_thread_contexts()
        {
            return std::thread::hardware_concurrency();
        }

#if defined(__WIN32) || defined(__UWP)
        WSYS_EXP static DWORD get_current_thread_id()
        {
            return GetCurrentThreadId();
        }
        WSYS_EXP static void sleep_current_thread(_In_ const DWORD& pMilliSecond)
        {
            Sleep(pMilliSecond);
        }
#else
        WSYS_EXP static size_t get_current_thread_id()
        {
            return std::hash<std::thread::id>{}(std::this_thread::get_id());
        }
        WSYS_EXP static void sleep_current_thread(_In_ const long long& pMilliSeconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(pMilliSeconds));
        }
#endif

    private:
        w_thread_pimp* _pimp;
    };
}
