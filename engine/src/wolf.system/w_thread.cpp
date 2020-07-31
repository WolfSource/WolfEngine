#include "w_system_pch.h"
#include "w_thread.h"

#ifdef __WIN32
#include <process.h>
#else
#include <mutex>
#include <condition_variable>
#include <sstream>
#endif

namespace wolf
{
    namespace system
    {
#if defined(__WIN32) || defined(__UWP)
		unsigned int __stdcall THREAD_WORK(_In_ void* pParameter)
		{
			auto _action_thread_ptr = (w_thread_pimp*)pParameter;
			if (_action_thread_ptr)
			{
				_action_thread_ptr->action();
				auto _thread_id = _action_thread_ptr->get_thread_id();
				_action_thread_ptr->destroy();
				logger.write("thread with id: {} destroyed", _thread_id);
			}
			return W_PASSED;
		}
#endif

    void w_thread_pimp::action()
    {
        //get thread id
#ifdef __WIN32
        this->_thread_id = w_thread::get_current_thread_id();
#else
        //convert std::thread::id to DWORD
        auto __thread_id = w_thread::get_current_thread_id();
        std::stringstream _ss;
        _ss << __thread_id;
        this->_thread_id = std::atoll(_ss.str().c_str());
        _ss.clear();
#endif
                    while (true)
                    {
    #ifdef __WIN32
                        EnterCriticalSection(&this->_critical_section);
                        if (!this->_job_queue.size())
                        {
                            SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, INFINITE);
                        }
                        if (this->_is_released)
                        {
                            break;
                        }
                        std::function<void()> _job = this->_job_queue.front();
                        LeaveCriticalSection(&this->_critical_section);

                        //execute job
                        _job();
                        
                        EnterCriticalSection(&this->_critical_section);
                        if (this->_is_released)
                        {
                            break;
                        }
                        if (this->_job_queue.size())
                        {
                            this->_job_queue.pop();
                        }
                        //signal for next action
                        WakeConditionVariable(&this->_condition_var);
                        LeaveCriticalSection(&this->_critical_section);
    #else
                        std::function<void()> _job;
                        
                        //for auto releasing _lock
                        {
                            std::unique_lock<std::mutex> _lock(this->_critical_section);
                            this->_condition_var.wait(_lock, [this]
                            {
                                return !this->_job_queue.empty() || this->_is_released;
                            });
                            
                            if (this->_is_released) break;
                            
                            _job = this->_job_queue.front();
                        }
                        
                        //execute job
                        _job();
                        
                         //for auto releasing _lock
                        {
                            std::lock_guard<std::mutex> _lock(this->_critical_section);
                            this->_job_queue.pop();
                            this->_condition_var.notify_one();
                        }
    #endif
                    }
                }

    }
}

using namespace wolf::system;
w_thread::w_thread() : _pimp(new w_thread_pimp())
{
}

w_thread::~w_thread()
{
    release();
}

void w_thread::add_job(_In_ const std::function<void()>& pJob)
{
    if (!this->_pimp) return;
    this->_pimp->add_job(pJob);
}

void w_thread::notify_one()
{
    if (!this->_pimp) return;
    this->_pimp->notify_one();
}

void w_thread::wait()
{
    if (!this->_pimp) return;
    this->_pimp->wait();
}

void w_thread::wait_until_complete()
{
    if (!this->_pimp) return;
    this->_pimp->wait_until_complete();
}

void w_thread::release()
{
    if (!this->_pimp) return;
    this->_pimp->release();
    this->_pimp = nullptr;
}
