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

namespace wolf
{
	namespace graphics
	{
#pragma pack(push, 1)
		struct quad_texture_uniform
		{
            //UV scale and invert property
			glm::vec2	        uv_scale = glm::vec2(1, 1);
            //opacity
			float				opacity = 1.0f;
            //don't care padding float
			float				padding;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct quad_color_uniform
		{
            //color
            glm::vec4	        color = glm::vec4(1, 1, 1, 1);
		};
#pragma pack(pop)

		template<typename T = quad_color_texture_uniform>
		class w_quad : public w_mesh
		{
		public:
            w_quad() :
                _texture(nullptr)
            {
                _super::set_class_name(typeid(this).name());
            }

			virtual ~w_quad()
			{
				release();
			}

            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_z_ const std::wstring& pTexture2DPath = L"",
                _In_z_ const std::wstring& pVertexShaderPath = L"",
                _In_z_ const std::wstring& pPixelShaderPath = L"")
            {
                //Load uniform/constant buffer
                this->_uniform.load(pDevice);

                using namespace wolf::content_pipeline::vertex_declaration_structs;

                const float _left = -1, _top = -1, _right = 1.0f, _down = 1.0f;

                const UINT _vertices_size = 4;
                const vertex_position_uv _vertices[_vertices_size] =
                {
                    { glm::vec3(_right	,  _down, 0.0f)	, glm::vec2(1.0f, 0.0f) },
                    { glm::vec3(_left	,  _top	, 0.0f)	, glm::vec2(0.0f, 1.0f) },
                    { glm::vec3(_left	,  _down, 0.0f)	, glm::vec2(0.0f, 0.0f) },
                    { glm::vec3(_right	,  _top	, 0.0f)	, glm::vec2(1.0f, 1.0f) }
                };

                const UINT _indices_size = 6;
                const UINT _indices[] = { 0, 1, 2, 0, 3, 1 };

                //create buffers
                auto _hr = _super::load(pDevice,
                    _vertices,
                    _vertices_size,
                    static_cast<UINT>(sizeof(vertex_position_uv)),
                    _indices,
                    _indices_size);

                if (_hr == S_FALSE)
                {
                    logger.error("Error on creating quad vertex and index buffers".);
                    return S_FALSE;
                }

                if (pTexture2DPath.empty())
                {
                    //we will use default texture
                    this->_texture = w_texture::default_texture;
                    logger.write(this->_texture->name);
                }
                else
                {

                }

                //	//create vertex shader
                //	std::wstring _v_shader_path = pVertexShaderPath;
                //	if (_v_shader_path.empty())
                //	{
                //		_v_shader_path = L"Shaders\\quad_vs.cso";
                //	}
                //	if (FAILED(_super::create_vertex_shader(pDevice, _v_shader_path)))  return S_FALSE;

                //	//create pixel shaders
                //	auto _ps_size = pPixelShaderPaths.size();
                //	if (_ps_size == 0)
                //	{
                //		if (FAILED(_super::create_pixel_shader(pDevice, L"Shaders\\quad_texture_ps.cso"))) return S_FALSE;
                //	}
                //	else
                //	{
                //		for (size_t i = 0; i < _ps_size; ++i)
                //		{
                //			if (FAILED(_super::create_pixel_shader(pDevice, pPixelShaderPaths[i]))) return S_FALSE;
                //		}
                //	}

                //	return _super::load_texture_2D_from_file(pDevice, pTexture2DPath, pIsAbsolutePath);

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

				return w_mesh::release();
			}

			w_uniform<T>		                _uniform;

		private:
			typedef	 w_mesh				        _super;
            wolf::graphics::w_texture*          _texture;
			
		};
	}
}

#endif