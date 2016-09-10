/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_system.h
	Description		 : system class of wolf
	Comment          :
*/

#ifndef __W_SYSTEM_H__
#define __W_SYSTEM_H__

#define WOLF_MajorVersion 0// when you make incompatible API changes.
#define WOLF_MinorVersion 2// when you add functionality in a backwards - compatible manner.
#define WOLF_PatchVersion 0// bug fixes
#define WOLF_DebugVersion 3// for debugging.

#ifndef W_UNUSED
#define W_UNUSED(p) {(void)(p); } 
#endif

#ifndef W_ARRAY_SIZE
#define W_ARRAY_SIZE(ARR)	(sizeof(ARR) / sizeof(ARR[0]))
#endif

#include "w_logger.h"

#endif