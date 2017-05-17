/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_quad.h
	Description		 : Render a quad with primitives
	Comment          :
*/

#ifndef __W_QUAD_H__
#define __W_QUAD_H__

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <w_vertex_declaration.h>
#include <w_graphics/w_uniform.h>
#include <w_graphics/w_texture.h>
#include "w_mesh.h"
#include "w_pipeline.h"

namespace wolf
{
	namespace graphics
	{
		class w_quad : public w_mesh
		{
		public:
            w_quad() :
                _texture(nullptr),
                _left(-1.0f),
                _top(-1.0f),
                _right(1.0f),
                _down(1.0f)
            {
                _super::set_class_name("w_quad");
            }

			virtual ~w_quad()
			{
				release();
			}

            HRESULT load(_In_ w_shader* pShader,
                         _In_ const w_render_pass* pRenderPass,
                         _In_ const std::string& pPipelineCacheName)
            {
                //Load uniform/constant buffer
                this->_uniform.load(pDevice);

                using namespace wolf::content_pipeline::vertex_declaration_structs;

                const std::vector<vertex_position_uv> _vertices =
                {
                    { glm::vec3(_right,  _down, 0.0f), glm::vec2(1.0f, 0.0f) },
                    { glm::vec3(_left ,  _top , 0.0f), glm::vec2(0.0f, 1.0f) },
                    { glm::vec3(_left ,  _down, 0.0f), glm::vec2(0.0f, 0.0f) },
                    { glm::vec3(_right,  _top , 0.0f), glm::vec2(1.0f, 1.0f) }
                };
                const std::vector<UINT> _indices = { 0, 1, 2, 0, 3, 1 };

                //create buffers
                auto _hr = _super::load(pDevice,
                    _vertices.data(),
                    _vertices.size(),
                    static_cast<UINT>(sizeof(vertex_position_uv)),
                    _indices.data(),
                    _indices.size());
                
                _vertices.clear();
                _indices.clear();

                if (_hr == S_FALSE)
                {
                    logger.error("Error on loading mesh of quad");
                    return S_FALSE;
                }

                if (pTexture2DPath.empty())
                {
                    //we will use default texture
                    this->_texture = w_texture::default_texture;
                }
                else
                {
                    _hr = w_texture::load_to_shared_textures(this->_gDevice, pTexture2DPath, &this->_texture);
                    if (_hr == S_FALSE)
                    {
                        logger.error("Error on loading texture for quad");
                    }
                }
                
                return S_OK;
            }

			//void render(_In_ ID3D11DeviceContext1* pContext, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			//{
			//	this->const_buffer.update(pContext);
			//	_super::update_shader_constant_buffer(pContext, 1, 1, this->const_buffer.get_buffer());
			//	_super::render(pContext, pPrimitiveTopology);
			//}

			//void render(_In_ ID3D11DeviceContext1* pContext, _In_ std::vector<ID3D11ShaderResourceView*> pShaderResourceViews, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			//{
			//	this->const_buffer.update(pContext);
			//	_super::update_shader_constant_buffer(pContext, 1, 1, this->const_buffer.get_buffer());
			//	_super::render(pContext, pShaderResourceViews, pPrimitiveTopology);
			//}

			virtual ULONG release() override
			{
				if (_super::get_is_released()) return 0;

				//release shader's constant/uniform buffers
				this->_uniform.release();
                this->_texture = nullptr;
                this->_gDevice = nullptr;

				return w_mesh::release();
			}

			w_uniform<T>		                _uniform;

#pragma region Setters
            
            void set_coordiantes(_In_ const float pLeft, _In_ const float pTop,
                _In_ const float pRight, _In_ const float pDown)
            {
                this->_left = pLeft;
                this->_top = pTop;
                this->_right = pRight;
                this->_down = pDown;
            }

#pragma endregion

		private:
			typedef	 w_mesh				        _super;
            std::shared_ptr<w_graphics_device>  _gDevice;
            wolf::graphics::w_texture*          _texture;
            float                               _left, _top, _right, _down;
		};
	}
}

#endif
