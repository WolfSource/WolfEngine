/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : pch.h
	Description		 : The main scene of Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include <w_game.h>
#include <w_camera.h>
#include <w_graphics\w_geometries\w_quad.h>

class scene sealed : public wolf::framework::w_game
{
public:
	scene();
	virtual ~scene();

	/*
		Allows the game to perform any initialization before starting to run.
		Calling Game::Initialize() will enumerate through any components and initialize them as well.
		The parameter pOutputWindowsInfo represents the information of output window(s) of this game.
	*/
	void initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) override;

	//The function "load()" will be called once per game and it is the place to load all of your game assets.
	void load() override;

	//This is the place where allows the game to run logic such as updating the world, checking camera, collisions, physics, input, playing audio and etc.
	void update(const wolf::system::w_game_time& pGameTime) override;

	//This is called when the game should draw itself.
	void render(const wolf::system::w_game_time& pGameTime) override;

	//Handle window messages
	HRESULT on_msg_proc(HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam) override;

	//Release will be called once per game and is the place to unload assets and release all resources
	ULONG release() override;

private:
	typedef wolf::framework::w_game														_super;

	wolf::content_pipeline::w_camera													_camera;
	//q_quad<> means use quad_const_buffer as default T
	std::unique_ptr<wolf::graphics::w_quad<>>											_quad;
};

#endif


