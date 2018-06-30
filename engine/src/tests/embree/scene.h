/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : https://WolfEngine.app
	Name			 : pch.h
	Description		 : The main scene of Wolf Engine
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __SCENE_H__
#define __SCENE_H__

#include <w_scene.h>

class scene : public wolf::render::embree::w_scene
{
public:
    scene(_In_z_ const std::wstring& pAppName, _In_z_ const std::wstring& pContentPath);
	virtual ~scene();

	/*
        Allows the game to perform any initialization and it needs to before starting to run.
        Calling w_game::Initialize() will enumerate through any components and initialize them as well.
        The parameter pOutputWindowInfo represents the information of output window(s) of this game.
	*/
    W_RESULT initialize(_In_ w_present_info& pPresentInfo) override;

	//The function "Load()" will be called once per game and is the place to load all of your game assets.
	void load() override;

	//This is the place where allows the game to run logic such as updating the world, checking camera, collisions, physics, input, playing audio and etc.
	void update(_In_ const wolf::system::w_game_time& pGameTime) override;

	//This is called when the game should draw itself.
	W_RESULT render(_In_ const wolf::system::w_game_time& pGameTime) override;

	//This is called when the window game should resized. pIndex is the index of window.
	void on_resized(_In_ const w_point& pNewSizeOfWindow) override;

	//This is called when the we lost graphics device.
	void on_device_lost() override;

	//Release function will be called once per game and is the place to unload assets and release all resources
	ULONG release() override;

private:
    bool															_update_gui();

};
#endif

