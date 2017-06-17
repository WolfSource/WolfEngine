/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : model.h
	Description		 : The model
	Comment          :
*/

#ifndef __MODEL_H__
#define __MODEL_H__

#include <w_graphics_device_manager.h>
#include <w_cpipeline_model.h>
#include <w_graphics/w_mesh.h>
#include <w_graphics/w_uniform.h>
#include <w_graphics/w_shader.h>
#include <w_graphics/w_pipeline.h>
#include <w_graphics/w_render_pass.h>

//struct clipspace_vertex { float x, y, z, w; };

class model : public wolf::system::w_object
{
public:
    W_EXP model();
    virtual ~model();

    HRESULT load(
        _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, 
        _In_ wolf::content_pipeline::w_cpipeline_model* pCPModel,
        _In_ wolf::graphics::w_render_pass& pRenderPass);

    HRESULT draw(_In_ const VkCommandBuffer& pCommandBuffer, _In_ VkDescriptorSet* pDescriptorSet);

    //Release will be called once per game and is the place to unload assets and release all resources
    ULONG release() override;

private:
    
    void _store_to_batch(
        _In_ const std::vector<wolf::content_pipeline::w_cpipeline_model::w_mesh*>& pModelMeshes,
        _Inout_ std::vector<float>& pVertices,
        _Inout_ std::vector<uint32_t>& pIndices,
        _Inout_ uint32_t& pBaseVertex);

    HRESULT model::_load_shader(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
    HRESULT  _load_buffers(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
    HRESULT  _load_pipelines(
        _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
        _In_ wolf::graphics::w_render_pass& pRenderPass);

    typedef wolf::system::w_object                          _super;

    std::string                                             _full_name;
    std::string                                             _search_name;

    wolf::content_pipeline::w_bounding_box                  _bounding_boxes;

    wolf::content_pipeline::w_transform_info               _transform;
    std::vector<wolf::content_pipeline::w_instance_info>   _instances_transforms;

    wolf::graphics::w_mesh*                                 _mesh;
    wolf::graphics::w_shader*                               _shader;

    //struct masked_occulusion_culling_data
    //{
    //    clipspace_vertex*   vertices = nullptr;
    //    uint32_t*           indices = nullptr;
    //    uint32_t            vertices_count = 0;
    //} _occulusion_culling_data;

    struct lod
    {
        uint32_t    first_index;
        uint32_t    index_count;
        float       distance;
        float       _padding;
    };
    std::vector<lod>                                        _lod_levels;

    wolf::graphics::w_vertex_binding_attributes             _vertex_binding_attributes;

    struct vertex_instance_data
    {
        glm::vec3   pos;
        glm::vec3   rot;
        float       scale;
    };

    struct vertex_unifrom
    {
        glm::mat4 projection_view;
    };

#pragma pack(push,1)
    struct compute_instance_data
    {
        glm::vec4   min_bounding_box;
        glm::vec4   max_bounding_box;
    };
#pragma pack(pop)

#pragma pack(push,1)
    struct compute_unifrom
    {
        glm::vec4 camera_pos;
        glm::vec4 frustum_planes[6];
    };
#pragma pack(pop)

#pragma pack(push,1)
    struct color_unifrom
    {
        glm::vec4 color = glm::vec4(1, 1, 1, 1);
    };
#pragma pack(pop)

    struct vertex_stage
    {
        wolf::graphics::w_uniform<vertex_unifrom>               unifrom;
        wolf::graphics::w_buffer                                instance_buffer;
        wolf::graphics::w_pipeline                              pipeline;
    } vs;

    struct compute_stage
    {
        uint32_t                                                batch_local_size = 1;
        wolf::graphics::w_uniform<compute_unifrom>              unifrom;
        wolf::graphics::w_buffer                                instance_buffer;

        wolf::graphics::w_buffer                                indirect_draw_commands_buffer;
        std::vector<VkDrawIndexedIndirectCommand>               indirect_draw_commands;
        wolf::graphics::w_buffer                                indirect_draw_count_buffer;

        wolf::graphics::w_command_buffers                       command_buffer;
        wolf::graphics::w_buffer                                lod_levels_buffers;
        VkFence                                                 fence = nullptr;//Synchronization fence to avoid rewriting compute command buffer if still in use
        VkSemaphore                                             semaphore = nullptr;;// Used as a wait semaphore for graphics submission

        wolf::graphics::w_pipeline                              pipeline;

        struct
        {
            uint32_t                                            draw_count;				  // Total number of indirect draw counts to be issued
            std::vector<uint32_t>                               lod_count;// Statistics for number of draws per LOD level (written by compute shader)
        } indirect_status;
    } cs;

};

#endif
