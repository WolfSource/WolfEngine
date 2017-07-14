#include "w_system_pch.h"
#include "w_thread.h"
#include <queue>
#include <process.h>

namespace wolf
{
    namespace system
    {
        unsigned int __stdcall THREAD_WORK(_In_ void* pParameter);

        class w_thread_pimp
        {
        public:
            w_thread_pimp() : _is_released(false)
            {
                InitializeCriticalSection(&this->_critical_section);
                InitializeConditionVariable(&this->_condition_var);
                this->_thread_handle = (HANDLE)_beginthreadex(0, 0, THREAD_WORK, this, 0, 0);
            }
            ~w_thread_pimp()
            {
                release();
            }

            void add_job(_In_ const std::function<void()>& pJob)
            {
                EnterCriticalSection(&this->_critical_section);
                this->_job_queue.push(std::move(pJob));
                //signal for action function
                WakeConditionVariable(&this->_condition_var);
                LeaveCriticalSection(&this->_critical_section);
            }

            void wait()
            {
                EnterCriticalSection(&this->_critical_section);
                while (!this->_job_queue.empty())
                {
                    SleepConditionVariableCS(&this->_condition_var, &this->_critical_section, INFINITE);
                }
                LeaveCriticalSection(&this->_critical_section);
            }

            void action()
            {
                this->_thread_id = w_thread::get_current_thread_id();
                while (true)
                {
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
                }
            }

            ULONG release()
            {
                if (this->_is_released) return 0;

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

                return 1;
            }

            void destroy()
            {
                //close thread
                CloseHandle(this->_thread_handle);
                DeleteCriticalSection(&this->_critical_section);
                delete this;
            }

            DWORD get_thread_id()
            {
                return this->_thread_id;
            }

        private:
#if defined(__WIN32) || defined(__UWP)
            HANDLE                                  _thread_handle;
            DWORD                                   _thread_id;
            CRITICAL_SECTION                        _critical_section;
            CONDITION_VARIABLE                      _condition_var;
#else
            
#endif
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
            return S_OK;
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