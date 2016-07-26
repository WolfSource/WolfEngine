/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_object.h
	Description		 : Base object class of Wolf clases
	Comment          :
*/

#ifndef __W_OBJECT_H__
#define __W_OBJECT_H__

#include <string>
#include "w_Ireleasable.h"

namespace wolf
{
	namespace system
	{
		class w_object : public w_Ireleasable
		{
		public:

			w_object() : _is_released(false), loadState(NOTLOADED) {}
			virtual ~w_object() { release(); }

			virtual bool is_released() { return this->_is_released; }
			virtual ULONG release() override { return _internal_release(); }

			void* tag;
			std::wstring name;

		protected:
			void set_class_name(const char* pClassName)
			{
				auto _name = std::string(pClassName);
				this->name = std::wstring(_name.begin(), _name.end());
				_name.clear();
			}

			enum LoadState { NOTLOADED, LOADING, LOADED };
			LoadState loadState;

		private:
			//Prevent copying
			w_object(w_object const&);
			w_object& operator= (w_object const&);
			
			ULONG _internal_release()
			{
				if (this->_is_released) return 0;

				this->_is_released = true;
				this->tag = nullptr;

#ifdef _DEBUG
				auto msg = L"\t\t[WOLF DEBUG] : " + std::wstring(this->name.begin(), this->name.end()) + L" released\r\n";
				OutputDebugStringW(msg.c_str());
				msg.clear();
#endif
				return 1;
			};
			

			bool _is_released;
		};
	}
}

#endif