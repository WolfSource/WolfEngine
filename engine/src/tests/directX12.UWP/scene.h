/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to contact@WolfSource.io
	Name			 : Scene.h
	Description		 : The main class of your game
	Comment          :
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include <w_game.h>

class scene sealed : public wolf::framework::w_game
{
public:
	scene();
	virtual ~scene();

	/*
		Allows the game to perform any initialization and it needs to before starting to run.
		Calling Game::Initialize() will enumerate through any components and initialize them as well.
		The parameter pOutputWindowsInfo represents the information of output window(s) of this game.
	*/
	void initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) override;

	//The function "Load()" will be called once per game and is the place to load all of your game assets.
	void load() override;

	//This is the place where allows the game to run logic such as updating the world, checking camera, collisions, physics, input, playing audio and etc.
	void update(_In_ const wolf::system::w_game_time& pGameTime) override;
	
	//Begin render on all graphics devices
	void begin_render(_In_ const wolf::system::w_game_time& pGameTime) override;

	//This is called when the game should draw itself.
	void render(_In_ const wolf::system::w_game_time& pGameTime) override;

	//End render on all graphics devices
	void end_render(_In_ const wolf::system::w_game_time& pGameTime) override;

	//This is called when the window game should resized. pIndex is the index of window.
	void on_window_resized(_In_ UINT pIndex) override;

	//This is called when the we lost graphics device.
	void on_device_lost() override;

	//Release will be called once per game and is the place to unload assets and release all resources
	ULONG release() override;

private:
	POINT																_global_mouse_point;
};

#endif

