/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_game.h
	Description		 : The main class of game
	Comment          :
*/

#pragma once

#include "w_graphics_device_manager.h"
#include <w_game_time.h>
#include <map>

namespace wolf::framework
{
	//Provides basic graphics device initialization, load game assets, game logic and rendering code.
	class w_game : public wolf::render::vulkan::w_graphics_device_manager
	{
	public:
		W_VK_EXP w_game(_In_z_ const std::wstring& pContentPath, _In_ const system::w_logger_config& pLogConfig);

		W_VK_EXP virtual ~w_game();

		//This will run the main loop cycle of the game
		W_VK_EXP bool run(_In_ std::map<int, w_present_info>& pOutputWindowsInfo);
		W_VK_EXP void exit();

		//Release all resources
		W_VK_EXP ULONG release();

#pragma region Setters

		W_VK_EXP void set_fixed_time_step(_In_ const bool& pValue) { this->_game_time.set_fixed_time_step(pValue); }
		W_VK_EXP void set_fixed_fps(_In_ const double& pValue)
		{
			this->_game_time.set_fixed_time_step(true);
			this->_game_time.set_target_elapsed_seconds(1.0 / pValue);
		}

#pragma endregion

#pragma region Getters

		W_VK_EXP bool get_is_released() const { return this->_is_released; };

#pragma endregion

	protected:
		/*
			Allows the game to perform any initialization before starting to run.
			Note that Log file will be created inside this function
		*/
		W_VK_EXP virtual void initialize(_In_ std::map<int, w_present_info> pOutputWindowsInfo) override;

		//Load game assets
		W_VK_EXP virtual void load() = 0;

		//Update the game logic
		W_VK_EXP virtual void update(_In_ const system::w_game_time& pGameTime);
		//Submit comand buffers on all graphics devices
		W_VK_EXP virtual W_RESULT render(_In_ const system::w_game_time& pGameTime);

		bool													exiting;
	private:
		typedef	 wolf::render::vulkan::w_graphics_device_manager		_super;
		bool															_is_released;
		bool															_loaded;
		wolf::system::w_game_time										_game_time;
		std::wstring													_app_name;

	};
}

#include "python_exporter/py_game.h"

