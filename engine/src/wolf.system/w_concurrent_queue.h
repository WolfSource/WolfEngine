/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_concurrent_queue.h
	Description		 : A cross platform concurrent queue, in windows inherited from Concurrency::concurrent_queue
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_CONCURRENT_QUEUE_H__
#define __W_CONCURRENT_QUEUE_H__

#if defined(__WIN32) || defined(__UWP)

#include <concurrent_queue.h>

#else defined(__APPLE__) || defined(__linux)

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#endif

namespace wolf
{
	namespace system
	{
#if defined(__WIN32) || defined(__UWP)
		template<typename _Ty, class _Ax = std::allocator<_Ty> >
		class w_concurrent_queue : public Concurrency::concurrent_queue<_Ty>
		{
		};
#else
		template<typename _Ty, class _Ax = std::allocator<_Ty> >
        class w_concurrent_queue
        {
        public:
			void clear()
			{
				std::unique_lock<std::mutex> _lock(this->_mutex);
				{
					this->_queue.clear();
				}
			}
			bool empty()
			{
				bool _result = false;
				std::unique_lock<std::mutex> _lock(this->_mutex);
				{
					_result = this->_queue.empty();
					_lock.unlock();
				}
				return _result;
			}

			void push(const _Ty& pItem)
            {
                //lock on queue
                std::unique_lock<std::mutex> _lock(this->_mutex);
                {
                    this->_queue.push(pItem);
                    _lock.unlock();
                }
                //then notify to conditional variable
                this->_cv.notify_one();
            }

			bool try_pop(_Ty& pItem)
			{
				bool _result = true;
				std::unique_lock<std::mutex> _lock(this->_mutex);
				{
					while (this->_queue.empty())
					{
						this->_cv.wait(_lock);
					}
					if (this->_queue.size())
					{
						pItem = this->_queue.front();
						this->_queue.pop();
					}
					else
					{
						_result = false;
					}
				}
				_lock.unlock();

				return _result;
			}

			_Ty unsafe_begin() const
			{
				return this->_queue.front();
			}

			_Ty unsafe_end() const
			{
				return this->_queue.back();
			}

            size_t unsafe_size() const
            {
                return this->_queue.size();
            }

            size_t safe_size() const
            {
                size_t _size = 0;
                std::unique_lock<std::mutex> _lock(this->_mutex);
                {   
                    _size = this->_queue.size();
                    _lock.unlock();
                }
                return _size;
            }

        private:
            std::queue<T>               _queue;
            std::mutex                  _mutex;
            std::condition_variable     _cv;
        };
#endif
	}
}

#endif //__W_CONCURRENT_QUEUE_H__