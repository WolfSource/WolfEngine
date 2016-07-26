/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_mesh.h
	Description		 : Render a mesh
	Comment          : 
*/
//TODO: move sprite class to another class. class Sprite : public Quad {

#ifndef __W_MESH_H__
#define __W_MESH_H__

#include <d3d11_2.h>
#include <DirectXMath.h>
#include <w_vertex_declaration.h>
#include "w_graphics\w_shaders\w_shader.h"
#include "w_graphics\w_shaders\w_constant_buffer.h"
#include "w_graphics\w_textures\w_texture_2D.h"

#pragma pack(push,1)
struct wvp_cBuffer
{
	DirectX::XMMATRIX	world_view_projection = DirectX::XMMATRIX(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);//Identity matrix
};
#pragma pack(pop)

#pragma pack(push, 1)
struct quad_cBuffer
{
	DirectX::XMFLOAT2	uv_scale = DirectX::XMFLOAT2(1, 1);
	float				opacity = 1.0f;
	float				fade_value = 0.5f;
	float				fade_smooth = 0.5f;
	DirectX::XMFLOAT3	padding;
};
#pragma pack(pop)

#pragma pack(push,1)
//Defenition of sprite object constant buffer
struct SpriteObjInfo
{
	DirectX::XMMATRIX	WorldViewProjection;
	float				CurrentFrame;
	float				SpritesPerRow;
	float				SpritesPerCol;
	float				Padding;
};
#pragma pack(pop)

namespace wolf
{
	namespace graphics
	{
		//Represents a 3D model mesh composed of multiple meshpart objects.
		class w_mesh : public system::w_object
		{
		public:
			DX_EXP w_mesh();
			DX_EXP virtual ~w_mesh();

			//Load a Identity quad
			DX_EXP HRESULT load(_In_ ID3D11Device1* pDevice, std::wstring pVertexShaderPath, std::wstring pPixelShaderPath, std::wstring pTexture2DPath, bool pLoadDeafultTexture2D,
				const wolf::content_pipeline::vertex_declaration_structs::vertex_position_normal_uv* const pVertices, const UINT pVerticesSize, 
				const unsigned short* const pIndices, const UINT pIndicesSize);
			//initialize default texture and assign to material of mesh
			DX_EXP HRESULT create_texture_2D(_In_ ID3D11Device1* pDevice, 
				_In_ UINT pWidth, 
				_In_ UINT pHeight,
				_In_ D2D1::ColorF pColor,
				_In_ D3D11_USAGE pUsage = D3D11_USAGE_DEFAULT, 
				_In_ UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlag = D3D11_BIND_SHADER_RESOURCE);

			//initialize default texture and assign to material of mesh
			DX_EXP HRESULT create_texture_2D(_In_ ID3D11Device1* pDevice,
				_In_ UINT pWidth,
				_In_ UINT pHeight,
				_In_ D3D11_USAGE pUsage = D3D11_USAGE_DEFAULT,
				_In_ UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlag = D3D11_BIND_SHADER_RESOURCE,
				_In_ D3D11_SUBRESOURCE_DATA* pInitialData = nullptr);

			//Load Texture2D from path and assign to material of mesh
			DX_EXP HRESULT load_texture_2D(_In_ ID3D11Device1* pDevice, std::wstring pPath);
			//Render mesh by using shader resource view
			DX_EXP void render(_In_ ID3D11DeviceContext1* pContext);
			//Render mesh by using shader resource view
			DX_EXP void render(_In_ ID3D11DeviceContext1* pContext, std::vector<ID3D11ShaderResourceView*> pSRVs);
			//Render srv over default texture
			DX_EXP void render_srv_over_default_texture(_In_ ID3D11DeviceContext1* pContext, ID3D11ShaderResourceView* pSRV, _In_ UINT pSRVSlotIndex = 1);

			//Release all resources
			DX_EXP virtual ULONG release() override;

#pragma region Getters

			//Set position of mesh
			DirectX::XMFLOAT3			get_position() const								{ return this->_position; }
			//Get Texture2D
			ID3D11Texture2D*			get_texture_2D() const								{ return this->_texture_2D->get_texture_2D(); }
			//Get resource of Texture2D
			ID3D11Resource*				get_resource() const								{ return this->_texture_2D->get_resource(); }
			//Get shader resource view of Texture2D
			ID3D11ShaderResourceView*	get_srv() const										{ return this->_texture_2D->get_srv(); }

#pragma endregion

#pragma region Setters

			//Enable/Disable sptite animation
			void						set_isSprite(bool pValue)							{ this->_is_sprite = pValue; }
			//Set position of mesh
			void						set_position(float pX, float pY, float pZ)			{ this->_position.x = pX; this->_position.y = pY; this->_position.z = pZ;}
			//Set position of mesh
			void						set_position(DirectX::XMFLOAT3 pValue)				{ this->_position = pValue; }
			//Set rotation of mesh
			void						set_rotation(float pX, float pY, float pZ)			{ this->_rotation.x = pX; this->_rotation.y = pY; this->_rotation.z = pZ; }
			//Set rotation of mesh
			void						set_rotation(DirectX::XMFLOAT3 pValue)				{ this->_rotation = pValue; }
			//Set scale of mesh
			void						set_scale(float pX, float pY, float pZ)				{ this->_scale.x = pX; this->_scale.y = pY; this->_scale.z = pZ;}
			//Set scale of mesh
			void						set_scale(DirectX::XMFLOAT3 pValue)					{ this->_scale = pValue; }
			//Set view matrix
			void						set_view_projection(DirectX::XMMATRIX pValue)		{ this->_view_projection = pValue; }
			//Set opacity
			void						set_opacity(float pValue)							{ this->_quad_cBuffer.data.opacity = pValue; }
			//Set uv
			void						set_uv_scale(float pX, float pY)					{ this->_quad_cBuffer.data.uv_scale.x = pX; this->_quad_cBuffer.data.uv_scale.y = pY;}
			//Set fade amount value
			void						set_fade_value(float pValue)						{ this->_quad_cBuffer.data.fade_value = pValue; }
			//Set fade smooth value
			void						set_fade_smooth_value(float pValue)					{ this->_quad_cBuffer.data.fade_smooth = pValue; }
			//Set current frame of sprite
			void						set_sprite_current_frame(UINT pCurrentFrame)		{ this->_sprite_obj_cBuffer.data.CurrentFrame = static_cast<float>(pCurrentFrame); }
			//Set number of sprites	per row
			void						set_sprites_per_row(UINT pSpritesPerRow)			{ this->_sprite_obj_cBuffer.data.SpritesPerRow = static_cast<float>(pSpritesPerRow); }
			//Set number of sprites	per column
			void						set_sprites_per_column(UINT pSpritesPerColumn)		{ this->_sprite_obj_cBuffer.data.SpritesPerCol = static_cast<float>(pSpritesPerColumn); }
			//Enable/Disable using identity matrix
			void						set_use_identity_matrix(bool pValue)				{ this->_use_identity_matrix = pValue; }
			
#pragma endregion
			
		private:
			typedef		system::w_object												_super;
			
			bool																		_is_sprite;
						
			bool																		_use_identity_matrix;
			DirectX::XMFLOAT3															_position;
			DirectX::XMFLOAT3															_rotation;
			DirectX::XMFLOAT3															_scale;
			DirectX::XMMATRIX															_world;
			DirectX::XMMATRIX															_view_projection;
			UINT																		_indices_size;
			UINT																		_stride;
			UINT																		_offset;
			UINT																		_texture_2D_width;
			UINT																		_texture_2D_height;

			w_constant_buffer<wvp_cBuffer>												_wvp_cBuffer;
			w_constant_buffer<quad_cBuffer>												_quad_cBuffer;
			
			w_constant_buffer<SpriteObjInfo>											_sprite_obj_cBuffer;

			std::unique_ptr<w_shader>													_shader;
			std::unique_ptr<w_texture_2D>												_texture_2D;
			Microsoft::WRL::ComPtr<ID3D11Buffer>										_vertex_buffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>										_index_buffer;
		};
	}
}

#endif