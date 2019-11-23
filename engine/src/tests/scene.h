/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : pch.h
	Description		 : The main scene of Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/
*/

#pragma once

#include <w_framework/w_game.h>

class scene : public wolf::framework::w_game
{
public:
    scene(_In_z_ const std::wstring& pContentPath, _In_ const wolf::system::w_logger_config& pLogConfig);
	virtual ~scene();

	/*
        Allows the game to perform any initialization and it needs to before starting to run.
        Calling w_game::Initialize() will enumerate through any components and initialize them as well.
        The parameter pOutputWindowsInfo represents the information of output window(s) of this game.
	*/
	void initialize(_In_ std::map<int, w_present_info> pOutputWindowsInfo) override;

	//The function "Load()" will be called once per game and is the place to load all of your game assets.
	void load() override;

	//This is the place where allows the game to run logic such as updating the world, checking camera, collisions, physics, input, playing audio and etc.
	void update(_In_ const wolf::system::w_game_time& pGameTime) override;

	//This is called when the game should draw itself.
	W_RESULT render(_In_ const wolf::system::w_game_time& pGameTime) override;

	//This is called when the window game should resized. pIndex is the index of window.
	void on_window_resized(_In_ const uint32_t& pGraphicsDeviceIndex, _In_ const w_point& pNewSizeOfWindow) override;

	//This is called when the we lost graphics device.
	void on_device_lost() override;

	//Release function will be called once per game and is the place to unload assets and release all resources
	ULONG release();

private:
    bool															_update_gui();

};

