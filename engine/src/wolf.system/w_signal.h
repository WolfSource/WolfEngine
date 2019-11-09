/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_event.h
	Description		 : A class for signal and slot
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_SIGNAL_H__
#define __W_SIGNAL_H__

#include <vector>

namespace wolf
{
	namespace system
	{
        template<typename sig>
        class w_signal;

        template<typename T, typename... Args>
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
            
            w_signal& operator = (w_slot pSlot)
            {
                this->_slots.clear();
                this->_slots.push_back(pSlot);
                return *this;
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

#endif //__W_EVENT_H__
