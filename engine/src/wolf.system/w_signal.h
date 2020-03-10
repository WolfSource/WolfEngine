/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_event.h
	Description		 : A class for signal and slot
	Comment          :
*/

#pragma once

#include <vector>
#include <functional>
#include <utility>

namespace wolf::system
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
			rise(std::forward<Args>(pArgs)...);
		}

		void rise(Args... pArgs) const
		{
		        if (this->_slots.empty())
			        return;
			for (auto i = this->_slots.begin(); i != this->_slots.end(); ++i)
			{
				(*i)(std::forward<Args>(pArgs)...);
			}
		}

		void remove(const size_t pIndex)
		{
			this->_slots.erase(pIndex);
		}

		void remove_all()
		{
			this->_slots.clear();
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

