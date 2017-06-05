/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_event.h
	Description		 : Base class for event
	Comment          : Define event's callback :
					   w_event<int> e;
					   e += [](int i) { cout<< i; };
					   Call it like : 
										e(100);
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_SIGNAL_H__
#define __W_SIGNAL_H__

#include <vector>

#ifdef __GNUC__
#pragma GCC visibility push(default) //The classes/structs below are exported
#endif

namespace wolf
{
	namespace system
	{
        template<class sig>
        class w_signal;

        template<class T, class... Args>
        class w_signal<T(Args...)>
        {
            typedef std::function<T(Args...)>   w_slot;

        public:
            void operator()(Args... pArgs)
            {
                emit(std::forward<Args>(pArgs)...);
            }

            void emit(Args... pArgs) const
            {
                for (auto i = this->_slots.begin(); i != this->_slots.end(); ++i)
                {
                    (*i)(std::forward<Args>(pArgs)...);
                }
            }

            w_signal& operator += (w_slot pSlot)
            {
                this->_slots.push_back(pSlot);
                return *this;
            }

            w_signal& operator -= (w_slot pSlot)
            {
                for (auto i = this->_slots.begin(); i != this->_slots.end(); ++i)
                {
                    if (*i == pSlot)
                    {
                        this->_slots.erase(i);
                        break;
                    }
                }
                return *this;
            }

        private:
            std::vector<w_slot>     _slots;
        };
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif


#endif //__W_EVENT_H__
