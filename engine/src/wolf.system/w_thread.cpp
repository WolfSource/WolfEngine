#include "w_system_pch.h"
#include "w_thread.h"
#include <queue>

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
        
#ifdef __WIN32
        unsigned int __stdcall THREAD_WORK(_In_ void* pParameter);
#endif
        
        class w_thread_pimp
        {
        public:
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

            void wait()
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

            void action()
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
                    wait();
                    this->_critical_section.lock();
                    this->_is_released = true;
                    this->_condition_var.notify_one();
                    this->_critical_section.unlock();
                    this->_thread_handle.join();
                    
                    logger.write("thread with id: " + std::to_string(_thread_id) + " destroyed");
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

#if defined(__WIN32) || defined(__UWP)
        unsigned int __stdcall THREAD_WORK(_In_ void* pParameter)
        {
            auto _action_thread_ptr = (w_thread_pimp*)pParameter;
            if (_action_thread_ptr)
            {
                _action_thread_ptr->action();
                auto _thread_id = _action_thread_ptr->get_thread_id();
                _action_thread_ptr->destroy();
                logger.write("thread with id: " + std::to_string(_thread_id) + " destroyed");
            }
            return W_PASSED;
        }
#endif

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

void w_thread::wait()
{
    if (!this->_pimp) return;
    this->_pimp->wait();
}

void w_thread::release()
{
    if (!this->_pimp) return;
    this->_pimp->release();
}
