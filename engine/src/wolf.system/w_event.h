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

#ifndef __W_EVENT_H__
#define __W_EVENT_H__

#include <vector>

#ifdef __GNUC__
#pragma GCC visibility push(default) //The classes/structs below are exported
#endif

namespace wolf
{
	namespace system
	{
		template<typename T>
		class w_event
		{
		public:
			typedef void(*func)(T);

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

			w_event& operator += (func pFunc)
			{
				this->_handlers.push_back(pFunc);
				return *this;
			}

			w_event& operator -= (func pFunc)
			{
				for (auto i = this->_handlers.begin(); i != this->_handlers.end(); ++i)
				{
					if (*i == pFunc)
					{
						this->_handlers.erase(i);
						break;
					}
				}
				return *this;
			}

		private:
			std::vector<func> _handlers;
		};
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif


#endif //__W_EVENT_H__
