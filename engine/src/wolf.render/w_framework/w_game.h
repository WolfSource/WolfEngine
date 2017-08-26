/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_game.h
	Description		 : The main class of game
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GAME_H__
#define __W_GAME_H__

#include "w_graphics_device_manager.h"
#include <w_game_time.h>
#include <map>

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace framework
	{
		//Provides basic graphics device initialization, load game assets, game logic and rendering code.
		class w_game : public wolf::graphics::w_graphics_device_manager
		{
		public:
            W_EXP w_game(_In_z_ const std::wstring& pRunningDirectory,
                         _In_z_ const std::wstring& pAppName = L"Wolf.Engine");
            
			W_EXP virtual ~w_game();

			//This will run the main loop cycle of the game
			W_EXP bool run(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo);
			W_EXP void exit(_In_ const int pExitCode = 0);

			//Release all resources
			W_EXP ULONG release() override;

#pragma region Setters

			W_EXP void set_fixed_time_step(_In_ bool pValue) { this->_game_time.set_fixed_time_step(pValue); }
			W_EXP void set_target_elapsed_ticks(_In_ UINT64 pValue) { this->_game_time.set_target_elapsed_ticks(10000000 / pValue); }
			W_EXP void set_target_elapsed_seconds(_In_ double pValue) { this->_game_time.set_target_elapsed_seconds(1.0 / pValue); }

#pragma endregion

#pragma region Getters


#pragma endregion

		protected:
			/*
				Allows the game to perform any initialization before starting to run.
				Note that Log file will be created inside this function
			*/
			W_EXP virtual void initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) override;

			//Load game assets
			W_EXP virtual void load() = 0;

			//Update the game logic
			W_EXP virtual void update(_In_ const system::w_game_time& pGameTime);
			//Submit comand buffers on all graphics devices
			W_EXP virtual HRESULT render(_In_ const system::w_game_time& pGameTime);

			bool													exiting;
		private:
			typedef	 wolf::graphics::w_graphics_device_manager      _super;
			wolf::system::w_game_time                               _game_time;

			std::wstring                                            _app_name;

		};
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
