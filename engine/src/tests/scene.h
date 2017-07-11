/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : scene.h
	Description		 : The main class of your game
	Comment          :
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include <w_cpipeline_scene.h>
#include <w_framework/w_game.h>
#include <w_framework/w_model.h>
#include <w_graphics/w_shader.h>
#include <w_graphics/w_uniform.h>
#include <w_graphics/w_command_buffers.h>
#include <w_graphics/w_render_pass.h>
#include <w_graphics/w_frame_buffers.h>
#include <w_graphics/w_pipeline.h>

#include <cameras/w_first_person_camera.h>
#include <w_point.h>

#include "model.h"

class scene : public wolf::framework::w_game
{
public:
#if defined(__WIN32)
	W_EXP scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName);
#elif defined(__UWP)
	W_EXP scene(_In_z_ const std::wstring& pAppName);
#else
	W_EXP scene(_In_z_ const std::string& pRunningDirectory, _In_z_ const std::string& pAppName);
#endif
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

    //Render on all graphics devices
    HRESULT render(_In_ const wolf::system::w_game_time& pGameTime) override;
        
    //This is called when the window game should resized. pIndex is the index of window.
    void on_window_resized(_In_ UINT pIndex) override;
    
    //This is called when the we lost graphics device.
    void on_device_lost() override;

    //Release will be called once per game and is the place to unload assets and release all resources
    ULONG release() override;

private:
    
    HRESULT _load_areas();
    HRESULT _build_draw_command_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
    HRESULT _build_gui_command_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
        _In_ const wolf::system::w_game_time& pGameTime);
    bool _update_gui();

    wolf::content_pipeline::w_first_person_camera                  _camera;
    std::vector<model*>                                            _models;

    wolf::graphics::w_command_buffers                              _draw_command_buffers;
    wolf::graphics::w_render_pass                                  _draw_render_pass;
    wolf::graphics::w_frame_buffers                                _draw_frame_buffers;

    wolf::graphics::w_command_buffers                              _gui_command_buffers;
    wolf::graphics::w_render_pass                                  _gui_render_pass;
    wolf::graphics::w_frame_buffers                                _gui_frame_buffers;
    

    w_point_t                                                      _screen_size;

    VkFence                                                        _compute_fence;//Synchronization fence to avoid rewriting compute command buffer if still in use


    VkSemaphore gui_semaphore;						// Used as a wait semaphore for graphics submission


    struct area
    {
        const char*                                                 area_name;

        wolf::content_pipeline::w_bounding_box                      inner_region;
        std::vector<model>                                          inner_models;

        wolf::content_pipeline::w_bounding_box                      middle_region;
        std::vector<model>                                          middle_models;

        wolf::content_pipeline::w_bounding_box                      outer_region;
        std::vector<model>                                          outer_models;
    };

    std::vector<area>                                               _areas;
};

#endif
