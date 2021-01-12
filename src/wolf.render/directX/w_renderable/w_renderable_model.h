/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_renderable_model.h
	Description		 : Render c_model using directX
	Comment          :
*/

#ifndef __W_RENDERABLE_MODEL_H__
#define __W_RENDERABLE_MODEL_H__

#include "w_graphics_device_manager.h"
#include <w_model.h>
#include <w_graphics/w_shaders/w_shader.h>
#include <w_graphics/w_shaders/w_constant_buffer.h>
#include <DirectXMath.h>

namespace wolf
{
	namespace graphics
	{
		namespace directX
		{
#pragma pack(push,1)
			struct model_cbuffer
			{
				//world view projection (transform) matrix
				DirectX::XMMATRIX	wvp = DirectX::XMMATRIX(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);//Identity matrix
			};
#pragma pack(pop)

			class w_renderable_model : public wolf::system::w_object
			{
			public:

				DX_EXP w_renderable_model(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, 
					wolf::content_pipeline::w_model* pModelData = nullptr);
				DX_EXP virtual ~w_renderable_model();
				
				DX_EXP HRESULT load();

				DX_EXP void render(const wolf::system::w_game_time& pGameTime);
				DX_EXP virtual ULONG release() override;

#pragma region Setters

				void set_view_projection(DirectX::XMMATRIX pValue);
				
#pragma endregion

			private:
				typedef	wolf::system::w_object _super;

				HRESULT _create_buffers(_In_ wolf::content_pipeline::w_model::w_mesh* pMesh);
				void _render_mesh(_In_ wolf::content_pipeline::w_model::w_mesh* pMesh, size_t pIndex);
				void _release_vertex_buffers();
				void _release_index_buffers();

				std::shared_ptr<wolf::graphics::w_graphics_device>			_gDevice;
				wolf::content_pipeline::w_model*							_base_model;
				DirectX::XMMATRIX											_view_projection;
				std::vector<ID3D11Buffer*>									_vertex_buffers;
				std::vector<ID3D11Buffer*>									_index_buffers;
				w_shader*													_shader;
				w_constant_buffer<model_cbuffer>							_model_cBuffer;
			};
		}
	}
}

#endif