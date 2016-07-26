/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_event.h
	Description		 : Base class for event
	Comment          : Define event's callback :
					   Event<int> e += [](int i) { cout<< i; };
					   Call it like : e(100);
*/

#ifndef __W_EVENT_H__
#define __W_EVENT_H__

#include <vector>
#include <functional>

namespace wolf
{
	namespace system
	{
		template<typename T>
		class w_event
		{
		public:
			typedef std::function<void(T)> Func;

		public:
			void call(T pArg)
			{
				for (auto i = this->_handlers.begin(); i != this->_handlers.end(); ++i)
				{
					(*i)(pArg);
				}
			}

			void operator ()(T pArg)
			{
				call(pArg);
			}

			w_event& operator += (Func pFunc)
			{
				this->_handlers.push_back(pFunc);
				return *this;
			}

			w_event& operator -= (Func pFunc)
			{
				for (auto i = this->_handlers.begin(); i != this->_handlers.end(); ++i)
				{
					if ((*i).target<void(T)>() == pFunc.target<void(T)>())
					{
						this->_handlers.erase(i);
						break;
					}
				}
				return *this;
			}

		private:
			std::vector<Func> _handlers;
		};
	}
}

#endif