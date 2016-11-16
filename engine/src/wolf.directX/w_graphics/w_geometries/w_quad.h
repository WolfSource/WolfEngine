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

#include "w_mesh.h"

namespace wolf
{
	namespace graphics
	{
#pragma pack(push, 1)
		struct quad_const_buffer
		{
			DirectX::XMFLOAT2	uv_scale = DirectX::XMFLOAT2(1, 1);
			float				opacity = 1.0f;
			float				padding;
		};
#pragma pack(pop)

		template<typename T = quad_const_buffer>
		class w_quad : public w_mesh
		{
		public:
			w_quad()
			{
				_super::set_class_name(typeid(this).name());
			}

			virtual ~w_quad()
			{
				release();
			}

			HRESULT load(_In_ ID3D11Device1* pDevice, _In_ bool pLoadDeafultTexture2D = true,
				_In_z_ const std::wstring& pTexture2DPath = L"", _In_ bool pIsAbsolutePath = false,
				_In_z_ const std::wstring& pVertexShaderPath = L"",
				_In_z_ const std::vector<std::wstring>& pPixelShaderPaths = std::vector<std::wstring>())
			{
				_super::initialize(pDevice);

				//Load constant buffer of quad
				this->_const_buffer.load(pDevice);

				using namespace wolf::content_pipeline::vertex_declaration_structs;

				const UINT _vertices_size = 4;
				const vertex_position_normal_uv _vertices[_vertices_size] =
				{
					{ glm::vec3(1.0f,  1.0f, 0.0f)	, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
					{ glm::vec3(-1.0f, -1.0f, 0.0f)	, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
					{ glm::vec3(-1.0f,  1.0f, 0.0f)	, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
					{ glm::vec3(1.0f, -1.0f, 0.0f)	, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }
				};

				const UINT _indices_size = 6;
				const unsigned short _indices[] = { 0, 1, 2, 0, 3, 1 };

				//create buffers
				if (FAILED(_super::create_vertex_buffer(pDevice, _vertices, _vertices_size, sizeof(vertex_position_normal_uv)))) return S_FALSE;
				if (FAILED(_super::create_index_buffer(pDevice, _indices, _indices_size))) return S_FALSE;
				//create texture buffer
				if (pLoadDeafultTexture2D)
				{
					if (FAILED(_super::create_default_texture_2D(pDevice))) return S_FALSE;
				}

				//create vertex shader
				std::wstring _v_shader_path = pVertexShaderPath;
				if (_v_shader_path.empty())
				{
					_v_shader_path = L"Shaders\\quad_vs.cso";
				}
				if (FAILED(_super::create_vertex_shader(pDevice, _v_shader_path)))  return S_FALSE;

				//create pixel shaders
				auto _ps_size = pPixelShaderPaths.size();
				if (_ps_size == 0)
				{
					if (FAILED(_super::create_pixel_shader(pDevice, L"Shaders\\quad_ps.cso"))) return S_FALSE;
				}
				else
				{
					for (size_t i = 0; i < _ps_size; ++i)
					{
						if (FAILED(_super::create_pixel_shader(pDevice, pPixelShaderPaths[i]))) return S_FALSE;
					}
				}

				return _super::load_texture_2D_from_file(pDevice, pTexture2DPath, pIsAbsolutePath);
			}

			void render(_In_ ID3D11DeviceContext1* pContext, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			{
				this->_const_buffer.update(pContext);
				_super::update_shader_constant_buffer(pContext, 1, 1, this->_const_buffer.get_buffer());
				_super::render(pContext, pPrimitiveTopology);
			}

			void render(_In_ ID3D11DeviceContext1* pContext, _In_ std::vector<ID3D11ShaderResourceView*> pShaderResourceViews, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			{
				this->_const_buffer.update(pContext);
				_super::update_shader_constant_buffer(pContext, 1, 1, this->_const_buffer.get_buffer());
				_super::render(pContext, pShaderResourceViews, pPrimitiveTopology);
			}

			virtual ULONG release() override
			{
				if (_super::is_released()) return 0;

				//Release shader's constant buffers
				this->_const_buffer.release();

				return w_mesh::release();
			}

#pragma region Getters
			//Get constant buffer data
			T							get_const_buffer_data() const				{ return this->_const_buffer.data; }
#pragma endregion

#pragma region Setters
			//Set constant buffer data
			void						set_const_buffer_data(_In_ const T& pData)	{ this->_const_buffer.data = pData; }
#pragma endregion


		private:
			typedef	 w_mesh				_super;
			w_constant_buffer<T>		_const_buffer;

		};
	}
}

#endif