#include "scene.h"
#include <w_graphics/w_shader.h>
#include <w_graphics/w_command_buffers.h>
#include <w_graphics/w_texture.h>
#include <w_graphics/w_shader_buffer.h>
#include <glm/glm.hpp>

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;

struct vertex_data
{
    float position[4];
    float uv[2];
};

scene::scene(_In_z_ std::string pRootDirectory, _In_z_ std::string pAppName) :
    w_game(pRootDirectory, pAppName)
{
    w_game::set_fixed_time_step(false);
}

scene::~scene()
{
    release();
}

void scene::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
    // TODO: Add your pre-initialization logic here
    w_game::initialize(pOutputWindowsInfo);
}

w_texture* _texture = nullptr;
w_shader_buffer<glm::mat4x4> _uniform;
w_shader _shader;
void scene::load()
{
    w_game::load();
    
    auto _gDevice =  this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    
    auto _hr = _uniform.load(_gDevice);
    
    _uniform.data = glm::mat4x4(0);
    _hr = _uniform.update();
    
    //load shaders
    _hr = _shader.load(_gDevice, "/Users/pooyaeimandar/Documents/github/Wolf.Engine/engine/src/wolf.render/shaders/shader_vertex_uv_uniform.vert.spv", w_shader_stage::VERTEX_SHADER);
    _hr = _shader.load(_gDevice, "/Users/pooyaeimandar/Documents/github/Wolf.Engine/engine/src/wolf.render/shaders/shader_uv.frag.spv", w_shader_stage::FRAGMENT_SHADER);
    
    std::vector<VkDescriptorPoolSize> _descriptor_pool_sizes =
    {
        {
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // Type
            1                                                   // DescriptorCount
        },
        {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // Type
            1                                                   // DescriptorCount
        }
    };
    _hr = _shader.create_descriptor_pool(_descriptor_pool_sizes);
    
    std::vector<VkDescriptorSetLayoutBinding> _layout_bindings =
    {
        {
            0,                                                  // Binding
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // DescriptorType
            1,                                                  // DescriptorCount
            VK_SHADER_STAGE_FRAGMENT_BIT,                       // StageFlags
            nullptr                                             // ImmutableSamplers
        },
        {
            1,                                                  // Binding
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // DescriptorType
            1,                                                  // DescriptorCount
            VK_SHADER_STAGE_VERTEX_BIT,                         // StageFlags
            nullptr                                             // ImmutableSamplers
        }
    };
    _hr = _shader.create_description_set_layout_binding(_layout_bindings);
    
    
    //load texture
    _texture = new w_texture();
    _hr = _texture->load(_gDevice);
    _hr = _texture->initialize_texture_2D_from_file("/Users/pooyaeimandar/Documents/github/Wolf.Engine/Logo.jpg", true);
    
    const VkDescriptorImageInfo _image_info = _texture->get_descriptor_info();
    const VkDescriptorBufferInfo _buffer_info = _uniform.get_descriptor_info();
    
    auto _descriptor_set = _shader.get_descriptor_set();
    std::vector<VkWriteDescriptorSet> _write_descriptor_sets =
    {
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
            nullptr,                                        // Next
            _descriptor_set,                                // DstSet
            0,                                              // DstBinding
            0,                                              // DstArrayElement
            1,                                              // DescriptorCount
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // DescriptorType
            &_image_info,                                   // ImageInfo
            nullptr,
        },
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
            nullptr,                                        // Next
            _descriptor_set,                                // DstSet
            1,                                              // DstBinding
            0,                                              // DstArrayElement
            1,                                              // DescriptorCount
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,              // DescriptorType
            nullptr,                                        // ImageInfo
            &_buffer_info,                                  // BufferInfo
            nullptr                                         // TexelBufferView
        }
    };
    _shader.update_descriptor_sets(_write_descriptor_sets);
    
    //create render pass
    _hr = _gDevice->create_render_pass("pass1");
    //create frame buffers
    _hr = _gDevice->create_frame_buffers_collection("frames",
                                              "pass1",
                                              _output_window->vk_swap_chain_image_views.size(),
                                              _output_window->vk_swap_chain_image_views.data(),
                                              _output_window->width,
                                              _output_window->height,
                                              1,
                                              _output_window->index);
    
    std::vector<vertex_data> _vertex_data =
    {
        { {-0.7f, -0.7f, 0.0f, 1.0f }, { -0.1f, -0.1f } },
        { {-0.7f,  0.7f, 0.0f, 1.0f }, { -0.1f,  1.1f } },
        { { 0.7f, -0.7f, 0.0f, 1.0f }, {  1.1f, -0.1f } },
        { { 0.7f,  0.7f, 0.0f, 1.0f }, {  1.1f,  1.1f } }
    };
    
    this->_mesh = new w_mesh();
    this->_mesh->load(_gDevice,
                     _vertex_data.data(),
                     static_cast<UINT>(_vertex_data.size() * sizeof(vertex_data)),
                     nullptr,
                     0,
                    true);
    
    std::vector<VkVertexInputBindingDescription> _vertex_binding_descriptions =
    {
        {
            0,                                                          // Binding
            sizeof(vertex_data),                                        // Stride
            VK_VERTEX_INPUT_RATE_VERTEX                                 // InputRate
        }
    };
    
    std::vector<VkVertexInputAttributeDescription> _vertex_attribute_descriptions =
    {
        {
            0,                                                          // Location
            _vertex_binding_descriptions[0].binding,                    // Binding
            VK_FORMAT_R32G32B32A32_SFLOAT,                              // Format
            offsetof(vertex_data, position)                             // Offset
        },
        {
            1,                                                          // Location
            _vertex_binding_descriptions[0].binding,                    // Binding
            VK_FORMAT_R32G32_SFLOAT,                                    // Format
            offsetof(vertex_data, uv)                                // Offset
        }
    };
    
    VkPipelineVertexInputStateCreateInfo _vertex_input_state_create_info =
    {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // Type
        nullptr,                                                      // Next
        0,                                                            // Flags
        static_cast<uint32_t>(_vertex_binding_descriptions.size()),   // VertexBindingDescriptionCount
        &_vertex_binding_descriptions[0],                             // VertexBindingDescriptions
        static_cast<uint32_t>(_vertex_attribute_descriptions.size()), // VertexAttributeDescriptionCount
        &_vertex_attribute_descriptions[0]                            // VertexAttributeDescriptions
    };
    
    VkPipelineInputAssemblyStateCreateInfo _input_assembly_state_create_info =
    {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,    // Type
        nullptr,                                                        // Next
        0,                                                              // Flags
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,                           // Topology
        VK_FALSE                                                        // Enable restart primitive
    };
    
    std::vector<VkDynamicState> _dynamic_states =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    
    VkPipelineDynamicStateCreateInfo _dynamic_state_create_info =
    {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,                   // Type
        nullptr,                                                                // Next
        0,                                                                      // Flags
        static_cast<uint32_t>(_dynamic_states.size()),                          // DynamicStateCount
        &_dynamic_states[0]                                                     // DynamicStates
    };
    
    //create view port
    w_viewport _wp;
    _wp.x = 0.0f;
    _wp.y = 0.0f;
    _wp.width = _output_window->width;
    _wp.height =  _output_window->height;
    _wp.minDepth = 0.0f;
    _wp.maxDepth = 1.0f;
    
    w_viewport_scissor _wp_sc;
    _wp_sc.offset.x = 0.0f;
    _wp_sc.offset.y = 0.0f;
    _wp_sc.extent.width = _output_window->width;
    _wp_sc.extent.height = _output_window->height;
    
    auto _descriptor_set_layout = _shader.get_descriptor_set_layout_binding();
    VkPipelineLayoutCreateInfo _pipeline_layout_create_info =
    {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // Type
        nullptr,                                        // Next
        0,                                              // Flags
        1,                                              // SetLayoutCount
        &_descriptor_set_layout,                        // SetLayouts
        0,                                              // PushConstantRangeCount
        nullptr                                         // PushConstantRanges
    };
    
    _hr = _gDevice->create_pipeline("pipeline1",
                                    "pass1",
                                    _shader.get_shader_stages(),
                                    { _wp }, //viewports
                                    { _wp_sc },
                                    &_pipeline_layout_create_info,
                                    &_vertex_input_state_create_info,
                                    &_input_assembly_state_create_info,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    &_dynamic_state_create_info);
    if (_hr)
    {
        logger.error("Error creating pipeline");
        return;
    }
    
    //now assign new command buffers
    this->_command_buffers = new w_command_buffers();
    this->_command_buffers->set_enable(true);
    this->_command_buffers->set_order(1);
    this->_command_buffers->load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    
    _hr = _gDevice->store_to_global_command_buffers("render_quad_with_texture",
                                                  this->_command_buffers,
                                                  _output_window->index);
    if (_hr)
    {
        logger.error("Error creating command buffer");
        return;
    }
    
    VkImageSubresourceRange _sub_resource_range = {};
    _sub_resource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    _sub_resource_range.baseMipLevel = 0;
    _sub_resource_range.levelCount = 1;
    _sub_resource_range.baseArrayLayer = 0;
    _sub_resource_range.layerCount = 1;
    
    VkClearValue _vk_clear_color =
    {
        { 1,0,0,1 }
    };

    _output_window->command_buffers.at("clear_color_screen")->set_enable(false);
    
    auto _command_buffers = _output_window->command_buffers.at("render_quad_with_texture");
    
    //record clear screen command buffer for every swap chain image
    for (uint32_t i = 0; i < _command_buffers->get_commands_size(); ++i)
    {
        this->_command_buffers->begin(i);
        {
            auto _command_buffer = _command_buffers->get_command_at(i);
        
            VkImageMemoryBarrier _present_to_render_barrier =
            {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,                                         // Type
                nullptr,                                                                        // Next
                VK_ACCESS_MEMORY_READ_BIT,                                                      // SrcAccessMask
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,                                           // DstAccessMask
                VK_IMAGE_LAYOUT_UNDEFINED,                                                      // OldLayout
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                                       // NewLayout
                _gDevice->vk_present_queue_family_index,                                        // SrcQueueFamilyIndex
                _gDevice->vk_graphics_queue_family_index,                // DstQueueFamilyIndex
                _output_window->vk_swap_chain_image_views[i].image,                             // Image
                _sub_resource_range                                                             // subresourceRange
            };
            
            vkCmdPipelineBarrier(_command_buffer,
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &_present_to_render_barrier);
        
            VkRenderPassBeginInfo _render_pass_begin_info =
            {
                VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType                sType
                nullptr,                                      // const void                    *pNext
                _gDevice->vk_render_passes["pass1"],                            // VkRenderPass                   renderPass
                _gDevice->vk_frame_buffers["frames"].at(i),                       // VkFramebuffer                  framebuffer
                {                                             // VkRect2D                       renderArea
                    {                                           // VkOffset2D                     offset
                        0,                                          // int32_t                        x
                        0                                           // int32_t                        y
                    },
                    {                                           // VkExtent2D                     extent
                        _output_window->width,                                        // int32_t                        width
                        _output_window->height,                                        // int32_t                        height
                    }
                },
                1,                                            // uint32_t                       clearValueCount
                &_vk_clear_color                                  // const VkClearValue            *pClearValues
            };
        
            auto _pipeline = _gDevice->vk_pipelines["pipeline1"];
            
            vkCmdBeginRenderPass( _command_buffer, &_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );
            vkCmdBindPipeline( _command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.pipeline);
    
            vkCmdSetViewport(_command_buffer, 0, 1, &_wp);
            vkCmdSetScissor(_command_buffer, 0, 1, &_wp_sc);
    
            VkDeviceSize _offset = 0;
            auto _vertex_buffer_handle = this->_mesh->get_vertex_buffer_handle();
            vkCmdBindVertexBuffers( _command_buffer, 0, 1, &_vertex_buffer_handle, &_offset );
        
            vkCmdBindDescriptorSets(_command_buffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    _pipeline.layout,
                                    0,
                                    1,
                                    &_descriptor_set,
                                    0,
                                    nullptr);

            vkCmdDraw( _command_buffer, static_cast<uint32_t>(_vertex_data.size()), 1, 0, 0 );
            vkCmdEndRenderPass( _command_buffer );
            
            _vertex_buffer_handle = nullptr;
        
            VkImageMemoryBarrier _barrier_from_render_to_present =
            {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,                     // Type
                nullptr,                                                    // Next
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,                       // SrcAccessMask
                VK_ACCESS_MEMORY_READ_BIT,                                  // DstAccessMask
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                   // OldLayout
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                            // NewLayout
                _gDevice->vk_graphics_queue_family_index,                    // SrcQueueFamilyIndex
                _gDevice->vk_present_queue_family_index,                   // DstQueueFamilyIndex
                _output_window->vk_swap_chain_image_views[i].image,         // Image
                _sub_resource_range                                         // SubresourceRange
            };
            
            vkCmdPipelineBarrier(_command_buffer,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &_barrier_from_render_to_present );
        }
        this->_command_buffers->end(i);
    }
}

static auto _jj = 0;
void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_game::exiting) return;
    
    if (_jj == 100)
    {
        _uniform.data = glm::mat4x4(1);
        auto _hr = _uniform.update();
    }
    
    _jj++;
    
    
    //logger.write(std::to_string(pGameTime.get_frames_per_second()));
    w_game::update(pGameTime);
}

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    return w_game::render(pGameTime);
}

void scene::on_window_resized(_In_ UINT pIndex)
{
    w_game::on_window_resized(pIndex);
}

void scene::on_device_lost()
{
    w_game::on_device_lost();
}

ULONG scene::release()
{
    if (this->get_is_released()) return 0;
    
    SAFE_RELEASE(_mesh);
    _texture->release();
    _uniform.release();
    _shader.release();
    
    return w_game::release();
}

