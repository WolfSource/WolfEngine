/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_IReleasable.h
	Description		 : An interface for Releasable
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_IRELEASABLE_H__
#define __W_IRELEASABLE_H__

#if defined (__ANDROID) || defined(__linux) || defined(__APPLE__)

#include "w_std.h"

#elif defined(__WIN32) || defined(__UWP)

#include <windows.h>

#define COM_RELEASE(x)			{ if (x) { x->Release(); x = nullptr;	} }
#define COMPTR_RELEASE(x)		{ if (x) { auto _x = x.GetAddressOf(); (*_x)->Release(); (*_x) = nullptr; x = nullptr; } }

#endif

#define SAFE_DELETE(x)			{ if (x) { delete x; x = nullptr;		} }
#define SAFE_DELETE_ARRAY(ar)   { if (ar){ delete[] ar; ar = nullptr;	} }

#define SAFE_DELETE_SHARED(x)   { if (x) { while(x.use_count() != 0) x.reset(); x = nullptr;} }

#define UNIQUE_RELEASE(x)		{ if (x) { x->release(); x.reset(nullptr); } }
#define SHARED_RELEASE(x)		{ if (x) { x->release(); while(x.use_count() != 0) x.reset(); x = nullptr;} }
#define SAFE_RELEASE(x)			{ if (x) { x->release(); delete x; x = nullptr;	} }

struct w_Ireleasable
{
	virtual ULONG release() = 0;
};


#endif //__W_IRELEASABLE_H__
