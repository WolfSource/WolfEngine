/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : blackmagic_decklink_dll.h
	Description		 : Dynamic library linkage header
	Comment          :
*/


#ifndef __W_BLACKMAGIC_DECKLINK_DLL_H__
#define __W_BLACKMAGIC_DECKLINK_DLL_H__

#ifdef _WIN32

	#ifdef __WOLF_DECKLINK__
	#define DLL __declspec(dllexport)
	#else
	#define DLL __declspec(dllimport)
	#endif

#endif

#endif