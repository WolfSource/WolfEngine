/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : https://WolfEngine.app
	Name			 : w_scene
	Description		 : the main class of scene
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_SCENE_H__
#define __W_SCENE_H__

#include "w_graphic_device_manager.h"
#include <w_game_time.h>
#include <w_point.h>
#include <vector>

namespace wolf
{
	namespace render
	{
        namespace embree
        {
		//provides basic graphics device initialization, load scene assets, scene logic and rendering code.
        class w_scene : public wolf::system::w_object
		{
		public:
            W_REN_EM w_scene(_In_z_ const std::wstring& pAppName, _In_z_ const std::wstring& pContentPath);
            
			W_REN_EM virtual ~w_scene();

			//this will run the main loop cycle of the game
			W_REN_EM bool run(_In_ w_present_info& pPresentInfo);
			W_REN_EM void exit();

			//release all resources
			W_REN_EM ULONG release() override;

#pragma region Setters

			W_REN_EM void set_fixed_time_step(_In_ const bool& pValue)     { this->_game_time.set_fixed_time_step(pValue); }
			W_REN_EM void set_fixed_fps(_In_ const double& pValue)
            {
                this->_game_time.set_fixed_time_step(true);
                this->_game_time.set_target_elapsed_seconds(1.0 / pValue);
            }

#pragma endregion

#pragma region Getters


#pragma endregion

		protected:
			/*
				allows the scene to perform any initialization before starting to run.
				note that Log file will be created inside this function
			*/
			W_REN_EM virtual W_RESULT initialize(_In_ w_present_info& pPresentInfo) = 0;
			//load game assets
			W_REN_EM virtual void load() = 0;
			//update the game logic
			W_REN_EM virtual void update(_In_ const system::w_game_time& pGameTime) = 0;
			//submit command buffers on all graphics devices
			W_REN_EM virtual W_RESULT render(_In_ const system::w_game_time& pGameTime) = 0;
            //this function called when the scene should resized
            W_REN_EM virtual void on_resized(_In_ const w_point& pNewSize) = 0;
            //this function called when the device lost
            W_REN_EM virtual void on_device_lost() = 0;
                bool													        exiting;
            
            RTCScene                                                            _rtc_scene;
              std::shared_ptr<w_graphic_device>                               _gDevice;
            private:
                typedef wolf::system::w_object                                  _super;
                wolf::system::w_game_time                                       _game_time;
                std::wstring                                                    _app_name;
            
            };
        }
	}
}

#endif


/*
 //Called when the APP suspends. It provides a hint to the driver that the APP is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
 W_REN_EM virtual void on_suspend();
 //Prepare frame on all graphics devices
 W_REN_EM virtual W_RESULT prepare();
 //Present on all graphics devices
 W_REN_EM virtual W_RESULT present();
 
 */
