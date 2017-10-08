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
#include <w_graphics/w_quad.h>
#include <w_media_core.h>
#include <w_memory.h>

#include <cameras/w_first_person_camera.h>
#include <w_point.h>

#include "model.h"
#include <w_thread_pool.h>

#include <tbb/tbb.h> 
#include <tbb/mutex.h>
#include <tbb/parallel_for_each.h>
#include <tbb/parallel_invoke.h>
#include <tbb/concurrent_vector.h>
#include <tbb/compat/condition_variable>

#define BUFFER_COUNTS			3
#define MAXIMUM_BUFFER_SIZE		10

class scene : public wolf::framework::w_game
{
public:
	W_EXP scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName);
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
    
    void    _load_area(_In_z_ const std::wstring& pArea, _In_ const bool pLoadCollada);
    HRESULT _load_areas();
    
    HRESULT _build_draw_command_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);

    HRESULT _build_gui_command_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
        _In_ const wolf::system::w_game_time& pGameTime);
    
    void    _async_seek(_In_ int64_t pSeekValue);
    bool    _update_gui();
    void    _update_media_player();
    HRESULT _open_media(_In_z_ const std::wstring& pPath, int64_t pSeekToFrame);
    void    _video_buffering_thread();
    void    _fill_buffers();
    void    _clear_buffers();

    wolf::graphics::w_viewport                                      _viewport;
    wolf::graphics::w_viewport_scissor                              _viewport_scissor;
    
    wolf::content_pipeline::w_first_person_camera                  _camera;
    bool                                                           _show_gui;

    wolf::graphics::w_command_buffers                              _draw_command_buffers;
    wolf::graphics::w_render_pass                                  _draw_render_pass;
    wolf::graphics::w_frame_buffers                                _draw_frame_buffers;
    
    wolf::graphics::w_command_buffers                              _gui_command_buffers;
    wolf::graphics::w_render_pass                                  _gui_render_pass;
    wolf::graphics::w_frame_buffers                                _gui_frame_buffers;
    

    w_point_t                                                      _screen_size;
    VkFence                                                        _draw_fence;
    VkSemaphore                                                     gui_semaphore;						// Used as a wait semaphore for graphics submission
    
    std::vector<model*>                                            _models_to_be_render;

    struct render_thread_context
    {
        wolf::system::w_thread                                      thread;
        wolf::graphics::w_command_buffers                           secondary_command_buffers;
        size_t                                                      batch_size;
        void release()
        {
            this->thread.release();
            this->secondary_command_buffers.release();
        }
    };
    std::vector<render_thread_context*>                                    _render_thread_pool;
    
    struct area
    {
        std::wstring                                                name = L"area";
        bool                                                        is_loaded = false;
        std::vector<wolf::content_pipeline::w_bounding_sphere*>     boundaries;

        std::vector<model*>                                         inner_models;
        std::vector<model*>                                         middle_models;
        std::vector<model*>                                         outer_models;

        uint32_t                                                    inner_loaded_index = 0;
        uint32_t                                                    middle_loaded_index = 0;
        uint32_t                                                    outer_loaded_index = 0;


        void release()
        {
            for (auto& _iter : this->inner_models)
            {
                SAFE_RELEASE(_iter);
            }
            this->inner_models.clear();

            for (auto& _iter : this->middle_models)
            {
                SAFE_RELEASE(_iter);
            }
            this->middle_models.clear();

            for (auto& _iter : this->outer_models)
            {
                SAFE_RELEASE(_iter);
            }
            this->outer_models.clear();

            this->boundaries.clear();
        }
    };

    tbb::concurrent_vector<area>                                    _areas;
    size_t                                                          _total_loaded_areas;
    size_t                                                          _total_areas;
    
    int64_t															_current_frame;
    int64_t															_video_frame_address;
    wolf::system::w_game_time										_video_time;
    wolf::system::w_memory											_video_buffers[BUFFER_COUNTS];
    tbb::atomic<bool>												_halt_buffering;
    tbb::mutex														_video_buffer_mutex;
    tbb::critical_section											_video_streamer_cs;
    wolf::framework::w_ffmpeg										_video_streamer;
    tbb::atomic<bool>												_video_is_buffering;
    tbb::interface5::condition_variable								_signal_video_buffer_cv;
    int																_current_video_buffer;
    tbb::atomic<size_t>												_total_video_frames_buffered;
    size_t															_total_video_frames;
    uint32_t                                                        _media_width;
    uint32_t                                                        _media_height;
    float                                                           _media_fps;
};

#endif
