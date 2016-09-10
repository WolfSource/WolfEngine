/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_IReleasable.h
	Description		 : An interface for Releasable
	Comment          :
*/

#ifndef __W_IRELEASABLE_H__
#define __W_IRELEASABLE_H__

#include <windows.h>

#define SAFE_DELETE(x)			{ if (x) { delete x; x = nullptr;		} }
#define SAFE_DELETE_ARRAY(ar)   { if (ar){ delete[] ar; ar = nullptr;	} }

#define SAFE_DELETE_SHARED(x)	{ if (x) { while(x.use_count() != 0) x.reset(); x = nullptr;} }

#define UNIQUE_RELEASE(x)		{ if (x) { x->release(); x.reset(nullptr); } }
#define SHARED_RELEASE(x)		{ if (x) { x->release(); while(x.use_count() != 0) x.reset(); x = nullptr;} }
#define SAFE_RELEASE(x)			{ if (x) { x->release(); delete x;	} }
#define COM_RELEASE(x)			{ if (x) { x->Release(); x = nullptr;	} }
#define COMPTR_RELEASE(x)		{ if (x) { auto _x = x.GetAddressOf(); (*_x)->Release(); (*_x) = nullptr; x = nullptr; } }

struct w_Ireleasable
{
	virtual ULONG release() = 0;
};


#endif