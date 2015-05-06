/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_Event.h
	Description		 : Base class for event
	Comment          : Define event's callback :
					   Event<int> e += [](int i) { cout<< i; };
					   Call it like : e(100);
*/

#pragma once

#include <vector>
#include <functional>

namespace Wolf
{
	namespace System
	{
		template<typename T>
		class W_Event
		{
		public:
			typedef std::function<void(T)> Func;

		public:
			void Call(T arg)
			{
				for (auto i = this->handlers.begin(); i != this->handlers.end(); ++i)
				{
					(*i)(arg);
				}
			}

			void operator ()(T arg)
			{
				Call(arg);
			}

			W_Event& operator += (Func f)
			{
				this->handlers.push_back(f);
				return *this;
			}

			W_Event& operator -= (Func f)
			{
				for (auto i = this->handlers.begin(); i != this->handlers.end(); ++i)
				{
					if ((*i).target<void(T)>() == f.target<void(T)>())
					{
						this->handlers.erase(i);
						break;
					}
				}
				return *this;
			}

		private:
			std::vector<Func> handlers;
		};
	}
}