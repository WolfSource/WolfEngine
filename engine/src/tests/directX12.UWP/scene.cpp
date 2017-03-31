/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to contact@WolfSource.io or tweet @Wolf_Engine on twitter
	Name			 : Scene.cpp
	Description		 : This is simple test, you can remove this project, during build
	Comment          :
*/

#include "pch.h"
#include "scene.h"
#include <d3d12.h>

using namespace std;
using namespace wolf::system;
using namespace wolf::framework;
using namespace wolf::graphics;

scene::scene()
{
	w_game::set_app_name(L"test.wolf.engine.directX12.UWP");
	w_game::set_fixed_time_step(false);

	//set the graphics config
	w_graphics_device_manager_configs _config = {};
	w_game::set_graphics_device_manager_configs(_config);
}

scene::~scene()
{
	release();
}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here
	w_game::initialize(pOutputWindowsInfo);
}

void scene::load()
{
	w_game::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
	logger.write(std::to_string(pGameTime.get_frames_per_second()));
	w_game::update(pGameTime);
}

void scene::begin_render(const wolf::system::w_game_time& pGameTime)
{
	w_game::begin_render(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	w_game::render(pGameTime);
}

void scene::end_render(const wolf::system::w_game_time& pGameTime)
{
	w_game::end_render(pGameTime);
}

void scene::on_window_resized(UINT pIndex)
{
	// TODO: Add your logic for on window resizing event
	w_game::on_window_resized(pIndex);
}

void scene::on_device_lost()
{
	// TODO: Add your logic for on device lost event
	w_game::on_device_lost();
}

ULONG scene::release()
{
	if (w_game::get_is_released()) return 0;

	return w_game::release();
}