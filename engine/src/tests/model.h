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

struct clipspace_vertex { float x, y, z, w; };

class model : public wolf::system::w_object
{
public:
    W_EXP model();
    virtual ~model();

    HRESULT load(
        _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, 
        _In_ wolf::content_pipeline::w_cpipeline_model* pCPModel,
        _In_ std::vector<wolf::content_pipeline::w_cpipeline_model*> pCPModelLODs);

    HRESULT draw(_In_ const VkCommandBuffer& pCommandBuffer, _In_ VkDescriptorSet* pDescriptorSet);

    //Release will be called once per game and is the place to unload assets and release all resources
    ULONG release() override;

private:
    HRESULT  _prepare_buffers(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);

    typedef wolf::system::w_object                          _super;

    std::string                                             _full_name;
    std::string                                             _search_name;

    std::vector<wolf::content_pipeline::w_bounding_box>     _bounding_boxes;

    wolf::content_pipeline::w_transform_info               _transform;
    std::vector<wolf::content_pipeline::w_instance_info>   _instances_transforms;

    wolf::graphics::w_mesh*                                 _mesh;

    struct masked_occulusion_culling_data
    {
        clipspace_vertex*   vertices = nullptr;
        uint32_t*           indices = nullptr;
        uint32_t            vertices_count = 0;
    } _occulusion_culling_data;

    struct lod
    {
        uint32_t    first_index;
        uint32_t    index_count;
        float       distance;
        float       _padding;
    };
    std::vector<lod>                                        _lod_levels;
};

#endif
