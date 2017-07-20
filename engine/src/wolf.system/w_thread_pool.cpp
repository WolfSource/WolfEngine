#include "w_system_pch.h"
#include "w_thread_pool.h"

using namespace wolf::system;

w_thread_pool::w_thread_pool()
{
}

w_thread_pool::~w_thread_pool()
{
    release();
}

void w_thread_pool::allocate(_In_ const size_t& pSize)
{
    release();
    this->_threads.resize(pSize);
}

void w_thread_pool::wait_for(_In_ const size_t& pThreadIndex)
{
    if (pThreadIndex < this->_threads.size())
    {
        this->_threads[pThreadIndex].wait();
    }
}

void w_thread_pool::wait_all()
{
    for (auto& _t : this->_threads)
    {
        _t.wait();
    }
}

void w_thread_pool::release()
{
    for (auto& _t : this->_threads)
    {
        _t.release();
    }
    this->_threads.clear();
}

#pragma region Getters

size_t w_thread_pool::get_pool_size() const
{
    return this->_threads.size();
}

#pragma endregion

#pragma region Setters

void w_thread_pool::add_jobs_for_thread(_In_ const size_t& pThreadIndex, _In_ const std::vector<std::function<void()>>& pJobs)
{
    if (pThreadIndex < this->_threads.size())
    {
        for (auto& _job : pJobs)
        {
            this->_threads[pThreadIndex].add_job(_job);
        }
    }
}

void w_thread_pool::add_job_for_thread(_In_ const size_t& pThreadIndex, _In_ const std::function<void()>& pJob)
{
    if (pThreadIndex < this->_threads.size())
    {
        this->_threads[pThreadIndex].add_job(pJob);
    }
}

#pragma endregion