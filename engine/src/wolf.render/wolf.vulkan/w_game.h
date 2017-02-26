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
            WVK_EXP w_game();
            WVK_EXP virtual ~w_game();

            //This will run the main loop cycle of the game
            WVK_EXP bool run(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo);
            WVK_EXP void exit(_In_ const int pExitCode = 0);

            //Release all resources
            WVK_EXP ULONG release() override;
			
#pragma region Setters

#if defined(__linux) || defined(__ANDROID) || defined(__APPLE__)
            WVK_EXP void set_app_name(_In_ std::string pAppName)			{ this->_app_name = pAppName; }
#else 
            WVK_EXP void set_app_name(_In_ std::wstring pAppName)			{ this->_app_name = pAppName; }
#endif
            WVK_EXP void set_fixed_time_step(_In_ bool pValue)				{ this->_game_time.set_fixed_time_step(pValue);}
            WVK_EXP void set_target_elapsed_ticks(_In_ UINT64 pValue)		{ this->_game_time.set_target_elapsed_ticks(10000000 / pValue); }
            WVK_EXP void set_target_elapsed_seconds(_In_ double pValue)     { this->_game_time.set_target_elapsed_seconds(1.0 / pValue); }

#pragma endregion

#pragma region Getters

#if defined(__linux) || defined(__ANDROID) || defined(__APPLE__)
            WVK_EXP static const std::string get_content_directory()        { return _content_directory_path; }
#else
            WVK_EXP static const std::wstring get_content_directory()       { return _content_directory_path; }
#endif
                        
#pragma endregion

    protected:

            /*
                Allows the game to perform any initialization before starting to run.
                Note that Log file will be created inside this function
             */
            WVK_EXP virtual void initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) override;

            //Load game assets
            WVK_EXP virtual void load() = 0;

            //Update the game logic
            WVK_EXP virtual void update(_In_ const system::w_game_time& pGameTime) = 0;

            //Begin render on all graphics devices
            WVK_EXP virtual void begin_render(_In_ const system::w_game_time& pGameTime);
			
            //Draw the components of the game, also when you call Game::Render(), it will execute all command queue
            WVK_EXP virtual void render(_In_ const system::w_game_time& pGameTime) = 0;

            //End render on all graphics devices
            WVK_EXP virtual void end_render(_In_ const system::w_game_time& pGameTime);

            //Handle window messages
            WVK_EXP virtual HRESULT on_msg_proc(/*_In_ HWND pHWND, _In_ UINT pMessage, _In_ WPARAM pWParam, _In_ LPARAM pLParam*/);

            bool exiting;
    private:
            typedef	 wolf::graphics::w_graphics_device_manager      _super;
            wolf::system::w_game_time                               _game_time;
                        
#if defined(__linux) || defined(__ANDROID) || defined(__APPLE__)
            std::string                                             _app_name;
            static std::string                                      _content_directory_path;
#else
            std::wstring                                            _app_name;
            static std::wstring                                     _content_directory_path;
#endif
			
		};
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
