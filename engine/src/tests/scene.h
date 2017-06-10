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

#define MAX_LOD_LEVEL 5

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
    
    void _prepare_buffers(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
    HRESULT _record_compute_command_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
    HRESULT _record_draw_command_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
    
#pragma pack(push,1)
    struct color_unifrom
    {
        glm::vec4 color = glm::vec4(1, 1, 1, 1);
    };
#pragma pack(pop)

    wolf::content_pipeline::w_first_person_camera                  _camera;
    wolf::graphics::w_command_buffers                              _draw_command_buffers;
    wolf::graphics::w_render_pass                                  _render_pass;
    wolf::graphics::w_frame_buffers                                _frame_buffers;

    wolf::graphics::w_shader*                                       _shader;

    wolf::graphics::w_pipeline*                                     _pipeline;
    wolf::graphics::w_pipeline*                                     _compute_pipeline;

    w_point_t                                                       _screen_size;

    struct model
    {
        std::vector<wolf::graphics::w_mesh*>                        model_meshes;
        bool                                                        has_instances = false;

        ULONG release()
        {
            for (auto& _mesh : this->model_meshes)
            {
                SAFE_RELEASE(_mesh);
            }
            return 1;
        }
    };
    std::vector<model*>                                             _models;
    
    struct vertex_instance_data 
    {
        glm::vec3   pos;
        glm::vec3   rot;
        float       scale;
    };

    struct compute_instance_data
    {
        glm::vec3   min_bounding_box;
        glm::vec3   max_bounding_box;
    };

    struct vertex_unifrom
    {
        glm::mat4 projection_view;
    };

    struct compute_unifrom
    {
        glm::vec4 cameraPos;
        glm::vec4 frustumPlanes[6];
    };


    wolf::graphics::w_uniform<vertex_unifrom>                      _vertex_unifrom;
    wolf::graphics::w_uniform<compute_unifrom>                      _compute_unifrom;

    // Contains the instanced data
    wolf::graphics::w_buffer vertex_instance_buffer;
    wolf::graphics::w_buffer compute_instance_buffer;

    // Contains the indirect drawing commands
    wolf::graphics::w_buffer indirectCommandsBuffer;
    wolf::graphics::w_buffer indirectDrawCountBuffer;

    // Indirect draw statistics (updated via compute)
    struct {
        uint32_t drawCount;						// Total number of indirect draw counts to be issued
        uint32_t lodCount[MAX_LOD_LEVEL + 1];	// Statistics for number of draws per LOD level (written by compute shader)
    } indirectStats;

    // Store the indirect draw commands containing index offsets and instance count per object
    std::vector<VkDrawIndexedIndirectCommand> indirectCommands;

    // Resources for the compute part of the example
    struct 
    {
        wolf::graphics::w_command_buffers           command_buffer;
        wolf::graphics::w_buffer lodLevelsBuffers;	// Contains index start and counts for the different lod levels
        VkFence fence;								// Synchronization fence to avoid rewriting compute CB if still in use
        VkSemaphore semaphore;						// Used as a wait semaphore for graphics submission
    } compute;
};

#endif
