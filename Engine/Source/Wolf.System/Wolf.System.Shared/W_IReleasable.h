/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_IReleasable.h
	Description		 : An interface for Releasable
	Comment          :
*/

#pragma once

#include <windows.h>

#define SAFE_DELETE(x)			{ if (x)			{ delete x; x = nullptr;		} }
#define SAFE_DELETE_ARRAY(ar)   { if (ar)			{ delete[] ar; ar = nullptr;	} }

#define SAFE_DELETE_SHARED(x)	{ if (x != nullptr) { while(x.use_count() != 0) x.reset(); x = nullptr;} }

#define UNIQUE_RELEASE(x)		{ if (x != nullptr) { x->Release(); x.release();	} }
#define SHARED_RELEASE(x)		{ if (x != nullptr) { x->Release(); while(x.use_count() != 0) x.reset(); x = nullptr;} }
#define COM_RELEASE(x)			{ if (x != nullptr) { auto _x = x.GetAddressOf(); (*_x)->Release();	(*_x) = nullptr; x = nullptr; } }

__interface W_IReleasable
{
	ULONG Release();
};
