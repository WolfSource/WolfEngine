/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_object.h
	Description		 : Base object class of Wolf clases
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_OBJECT_H__
#define __W_OBJECT_H__

#if defined(__WIN32) || defined(__UWP)
#include <string>
#endif

#include "w_ireleasable.h"
#include "w_system_export.h"

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace system
	{
        //forward declaration
        class w_object_pimp;
		class w_object : public w_Ireleasable
		{
		public:

			WSYS_EXP w_object();
			WSYS_EXP virtual ~w_object() { release(); }

			WSYS_EXP virtual ULONG release() override;
			WSYS_EXP virtual bool get_is_released();
            
			//we are not releasing your tag
			void* tag;
			std::string name;

		protected:
			WSYS_EXP void set_class_name(const char* pClassName);

			enum LoadState { NOTLOADED, LOADING, LOADED };
			LoadState loadState;

		private:
			//prevent copying
			w_object(w_object const&);
			w_object& operator= (w_object const&);
        
            w_object_pimp* _pimp;
		};
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_OBJECT_H__
