/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_quad.h
	Description		 : Render a quad with primitives
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once

#endif
#ifndef __W_QUAD_H__
#define __W_QUAD_H__

#include <w_graphics_device_manager.h>
#include <w_graphics/w_mesh.h>
#include <w_graphics/w_shader.h>
#include <w_graphics/w_buffer.h>
#include <w_graphics/w_command_buffers.h>
#include <w_vertex_declaration.h>

namespace wolf
{
	namespace graphics
	{
        template<typename T = wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv>
        class w_quad : public w_mesh
		{
		public:
            w_quad() : 
                _left(-1.0f),
                _top(-1.0f),
                _right(1.0f),
                _down(1.0f),
                _left_top_color(w_color::WHITE()),
                _right_bottom_color(w_color::YELLOW()),
                _left_bottom_color(w_color::BLACK()),
                _right_top_color(w_color::GREEN())
            {
                _super::set_class_name("w_quad");
            }

            ~w_quad()
            {
                release();
            }

            HRESULT load(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                _In_ w_shader* pShader,
                _In_ const w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName,
                _In_ const bool pUseDynamicBuffer)
            {
                using namespace wolf::content_pipeline;
                
                this->_gDevice = pGDevice;

                std::vector<float> _vertices;

                if (std::is_same<T, vertex_declaration_structs::vertex_position>::value)
                {
                    //v1
                    _vertices.push_back(_right); _vertices.push_back(_down); _vertices.push_back(0);

                    //v2
                    _vertices.push_back(_left); _vertices.push_back(_top); _vertices.push_back(0);

                    //v3
                    _vertices.push_back(_left); _vertices.push_back(_down); _vertices.push_back(0);

                    //v4
                    _vertices.push_back(_right); _vertices.push_back(_top); _vertices.push_back(0);

                    this->_vertex_declaration = w_vertex_declaration::VERTEX_POSITION;
                }
                else if (std::is_same < T, vertex_declaration_structs::vertex_position_uv>::value)
                {
                    //v1
                    _vertices.push_back(_right); _vertices.push_back(_down); _vertices.push_back(0);
                    _vertices.push_back(1.0f); _vertices.push_back(0.0f);

                    //v2
                    _vertices.push_back(_left); _vertices.push_back(_top); _vertices.push_back(0);
                    _vertices.push_back(0.0f); _vertices.push_back(1.0f);
                    
                    //v3
                    _vertices.push_back(_left); _vertices.push_back(_down); _vertices.push_back(0);
                    _vertices.push_back(0.0f); _vertices.push_back(0.0f);
                    
                    //v4
                    _vertices.push_back(_right); _vertices.push_back(_top); _vertices.push_back(0);
                    _vertices.push_back(1.0f); _vertices.push_back(1.0f);
                    
                    this->_vertex_declaration = w_vertex_declaration::VERTEX_POSITION_UV;
                }
                else if (std::is_same < T, vertex_declaration_structs::vertex_position_uv_color>::value)
                {
                    //v1
                    _vertices.push_back(_right); _vertices.push_back(_down); _vertices.push_back(0);
                    _vertices.push_back(this->_right_bottom_color.r / 255.0f); _vertices.push_back(this->_right_bottom_color.g / 255.0f); _vertices.push_back(this->_right_bottom_color.b / 255.0f); _vertices.push_back(this->_right_bottom_color.a / 255.0f);
                    _vertices.push_back(0.0f); _vertices.push_back(1.0f);

                    //v2
                    _vertices.push_back(_left); _vertices.push_back(_top); _vertices.push_back(0);
                    _vertices.push_back(this->_left_top_color.r / 255.0f); _vertices.push_back(this->_left_top_color.g / 255.0f); _vertices.push_back(this->_left_top_color.b / 255.0f); _vertices.push_back(this->_left_top_color.a / 255.0f);
                    _vertices.push_back(1.0f); _vertices.push_back(0.0f);

                    //v3
                    _vertices.push_back(_left); _vertices.push_back(_down); _vertices.push_back(0);
                    _vertices.push_back(this->_left_bottom_color.r / 255.0f); _vertices.push_back(this->_left_bottom_color.g / 255.0f); _vertices.push_back(this->_left_bottom_color.b / 255.0f); _vertices.push_back(this->_left_bottom_color.a / 255.0f);
                    _vertices.push_back(1.0f); _vertices.push_back(1.0f);

                    //v4
                    _vertices.push_back(_right); _vertices.push_back(_top); _vertices.push_back(0);
                    _vertices.push_back(this->_right_top_color.r / 255.0f); _vertices.push_back(this->_right_top_color.g / 255.0f); _vertices.push_back(this->_right_top_color.g / 255.0f); _vertices.push_back(this->_right_top_color.a / 255.0f);
                    _vertices.push_back(0.0f); _vertices.push_back(0.0f);

                    this->_vertex_declaration = w_vertex_declaration::VERTEX_POSITION_UV_COLOR;
                }
                else
                {
                    V(S_FALSE, L"Unsupported vertex layout for quad ", this->name, 3);
                    return S_FALSE;
                }

                const std::vector<UINT> _indices = { 0, 1, 2, 0, 3, 1 };

                //load mesh
                this->_mesh = new (std::nothrow) w_mesh();
                if (!this->_mesh)
                {
                    V(S_FALSE, "allocating memory for mesh", this->name, 3);
                    return S_FALSE;
                }
                //_mesh->set_vertex_declaration_struct(this->_instances_count ? this->_instance_declaration :
                //    this->_vertex_declaration);
                /*auto _hr = _mesh->load(this->_gDevice,
                    _vertices.data(),
                    static_cast<uint32_t>(_vertices.size()),
                    static_cast<uint32_t>(_vertices.size() * sizeof(float)),
                    _indices.data(),
                    static_cast<uint32_t>(_indices.size(),
                    pUseDynamicBuffer);

                if (_hr == S_FALSE)
                {
                    logger.error(L"Could not create mesh for quad");
                    return S_FALSE;
                }*/

                return S_OK;
            }

            HRESULT update_instance_buffer(_In_ const void* const pInstancedData,
                _In_ const UINT pInstancedSize, _In_ const w_vertex_declaration pInstanceVertexDeclaration)
            {
                using namespace wolf::graphics;

                if (!pInstancedData || pInstancedSize == 0) return S_FALSE;

                this->_instances_count = pInstancedSize;
                this->_instance_declaration = pInstanceVertexDeclaration;

                HRESULT _hr;

#pragma region staging buffer in DRAM
                w_buffer _staging_buffer;
                _hr = _staging_buffer.load_as_staging(this->_gDevice, pInstancedSize);
                if (_hr == S_FALSE)
                {
                    logger.error("Could not create staging instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }

                _hr = _staging_buffer.set_data(pInstancedData);
                if (_hr == S_FALSE)
                {
                    logger.error("Setting staging instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }

                _hr = _staging_buffer.bind();
                if (_hr == S_FALSE)
                {
                    logger.error("Could not bind to staging instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }
#pragma endregion

#pragma region create VRAM buffer

                if (_instances_buffer == nullptr)
                {
                    this->_instances_buffer = new (std::nothrow) w_buffer();
                    if (!this->_instances_buffer)
                    {
                        logger.error("Error on creating instance buffer.");
                        return S_FALSE;
                    }

                    _hr = this->_instances_buffer->load(this->_gDevice,
                        pInstancedSize,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    if (_hr == S_FALSE)
                    {
                        logger.error("Could not create instance buffer of model: " +
                            this->_pipeline_model->get_name());
                        return _hr;
                    }
                }
#pragma endregion

                //bind instance buffer
                _hr = this->_instances_buffer->bind();
                if (_hr == S_FALSE)
                {
                    logger.error("Could not bind to instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }

                //create one command buffer
                auto _copy_command_buffer = new w_command_buffers();
                _copy_command_buffer->load(this->_gDevice, 1);

                _copy_command_buffer->begin(0);
                {
                    auto _copy_cmd = _copy_command_buffer->get_command_at(0);

                    VkBufferCopy _copy_region = {};

                    _copy_region.size = pInstancedSize;
                    vkCmdCopyBuffer(_copy_cmd,
                        _staging_buffer.get_handle(),
                        this->_instances_buffer->get_handle(),
                        1,
                        &_copy_region);
                }
                _copy_command_buffer->flush(0);

                SAFE_DELETE(_copy_command_buffer);

                _staging_buffer.release();

                return S_OK;
            }
            
            void render(_In_ const VkCommandBuffer& pCommandBuffer)
            {
                if (!this->_mesh) return;

                auto _instance_handle = this->_instances_buffer ? this->_instances_buffer->get_handle() : nullptr;
                this->_mesh->render(pCommandBuffer, _instance_handle, this->_instances_count);
            }
            
            virtual ULONG release() override
            {
                if (_super::get_is_released()) return 0;

                SAFE_RELEASE(this->_mesh);
                SAFE_RELEASE(this->_instances_buffer);
                this->_gDevice = nullptr;

                return _super::release();
            }

#pragma region Getters

            UINT get_instances_count() const
            {
                return this->_instances_count;
            }

#pragma endregion

#pragma region Setters
            
            void set_position(_In_ const float pLeft, _In_ const float pTop,
                _In_ const float pRight, _In_ const float pDown)
            {
                this->_left = pLeft;
                this->_top = pTop;
                this->_right = pRight;
                this->_down = pDown;
            }

            void set_left_color(_In_ w_color pColor)
            {
                this->_left_top_color[0] = pColor.r / 255.0f;
                this->_left_top_color[1] = pColor.g / 255.0f;
                this->_left_top_color[2] = pColor.b / 255.0f;
                this->_left_top_color[3] = pColor.a / 255.0f;
            }

            void set_top_color(_In_ w_color pColor)
            {
                this->_left_bottom_color[0] = pColor.r / 255.0f;
                this->_left_bottom_color[1] = pColor.g / 255.0f;
                this->_left_bottom_color[2] = pColor.b / 255.0f;
                this->_left_bottom_color[3] = pColor.a / 255.0f;
            }

            void set_right_color(_In_ w_color pColor)
            {
                this->_right_bottom_color[0] = pColor.r / 255.0f;
                this->_right_bottom_color[1] = pColor.g / 255.0f;
                this->_right_bottom_color[2] = pColor.b / 255.0f;
                this->_right_bottom_color[3] = pColor.a / 255.0f;
            }

            void set_down_color(_In_ w_color pColor)
            {
                this->_right_top_color[0] = pColor.r / 255.0f;
                this->_right_top_color[1] = pColor.g / 255.0f;
                this->_right_top_color[2] = pColor.b / 255.0f;
                this->_right_top_color[3] = pColor.a / 255.0f;
            }

#pragma endregion

		private:
			typedef	 w_mesh				                        _super;

            std::shared_ptr<w_graphics_device>                  _gDevice;
            wolf::graphics::w_mesh*                             _mesh;
            float                                               _left, _top, _right, _down;
            w_color                                             _left_top_color;
            w_color                                             _left_bottom_color;
            w_color                                             _right_bottom_color;
            w_color                                             _right_top_color;
            wolf::graphics::w_buffer*                           _instances_buffer;
            UINT                                                _instances_count;
            w_vertex_declaration                                _vertex_declaration;
            w_vertex_declaration                                _instance_declaration;
		};
	}
}

#endif
