/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_game.h
	Description		 : The main class of game
	Comment          :
*/

#ifndef __W_GAME_H__
#define __W_GAME_H__

#include "w_graphics_device_manager.h"
#include "w_graphics\w_sprite_batch.h"

#include <w_game_time.h>

#ifdef __MAYA
#include "GraphicsResource/Textures/RenderTarget2D.h"
#endif // MAYA

namespace wolf
{
	namespace framework
	{
		//Provides basic graphics device initialization, load game assets, game logic and rendering code.
		class w_game : public wolf::graphics::w_graphics_device_manager
		{
		public:
			DX_EXP w_game();
			DX_EXP virtual ~w_game();

			//This will run the main loop cycle of the game 
			DX_EXP bool run(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo);
			DX_EXP void exit(_In_ const int pExitCode = 0);

			//Release all resources
			DX_EXP ULONG release() override;
			
#pragma region Setters

			void set_app_name(_In_ std::wstring pAppName)								{ this->_app_name = pAppName; }
			void set_fixed_time_step(_In_ bool pValue)									{ this->_game_time.set_fixed_time_step(pValue);}
			void set_target_elapsed_ticks(_In_ UINT64 pValue)							{ this->_game_time.set_target_elapsed_ticks(10000000 / pValue); }
			void set_target_elapsed_seconds(_In_ double pValue)							{ this->_game_time.set_target_elapsed_seconds(1.0 / pValue); }

#pragma endregion

#pragma region Getters

			DX_EXP static const std::wstring get_content_directory()					{ return _content_directory_path; }

#pragma endregion

		protected:

			/*
				Allows the game to perform any initialization before starting to run.
				Note that Log file will be created inside this function
			*/
			DX_EXP virtual void initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) = 0;

			//Load game assets
			DX_EXP	virtual void load() = 0;

			//Update the game logic
			DX_EXP virtual void update(_In_ const system::w_game_time& pGameTime) = 0;

			//Begin render on all graphics devices
			DX_EXP virtual void begin_render(_In_ const system::w_game_time& pGameTime);
			
			//Draw the components of the game, also when you call Game::Render(), it will execute all command queue
			DX_EXP virtual void render(_In_ const system::w_game_time& pGameTime) = 0;

			//End render on all graphics devices
			DX_EXP virtual void end_render(_In_ const system::w_game_time& pGameTime);

			//Handle window messages
			DX_EXP virtual HRESULT on_msg_proc(_In_ HWND pHWND, _In_ UINT pMessage, _In_ WPARAM pWParam, _In_ LPARAM pLParam);

			//SpriteBatch object for debugging 
			std::unique_ptr<wolf::graphics::w_sprite_batch>		sprite_batch;

			bool exiting;
		private:
			typedef	 wolf::graphics::w_graphics_device_manager   _super;

			std::wstring										_app_name;
			wolf::system::w_game_time							_game_time;
			static std::wstring									_content_directory_path;

#ifdef __MAYA
			std::unique_ptr<Wolf::Graphics::RenderTarget2D>		backBufferRenderTarget;
#endif // MAYA
		};
	}
}

#endif