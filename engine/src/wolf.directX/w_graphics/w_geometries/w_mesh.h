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
#include "w_directX_math_helper.h"
#include "w_graphics\w_shaders\w_shader.h"
#include "w_graphics\w_shaders\w_constant_buffer.h"
#include "w_graphics\w_textures\w_texture_2D.h"

#pragma pack(push,1)
struct wvp_const_buffer
{
	DirectX::XMMATRIX	world_view_projection = DirectX::XMMATRIX(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);//Identity matrix
};
#pragma pack(pop)

//#pragma pack(push,1)
////Defenition of sprite object constant buffer
//struct SpriteObjInfo
//{
//	DirectX::XMMATRIX	WorldViewProjection;
//	float				CurrentFrame;
//	float				SpritesPerRow;
//	float				SpritesPerCol;
//	float				Padding;
//};
//#pragma pack(pop)

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

			//Initialize mesh
			DX_EXP void initialize(_In_ ID3D11Device1* pDevice);
			//load and create vertex shader
			DX_EXP HRESULT create_vertex_shader(_In_ ID3D11Device1* pDevice, _In_z_ const std::wstring pPath);
			//load and create pixel shader
			DX_EXP HRESULT create_pixel_shader(_In_ ID3D11Device1* pDevice, _In_z_ const std::wstring pPath);

			//create vertex shader
			DX_EXP HRESULT create_vertex_buffer(_In_ ID3D11Device1* pDevice,
				_In_ const void* const pVertices,
				_In_ const UINT pVerticesSize,
				_In_ const UINT pStride,
				_In_ D3D11_USAGE pUsageFlag = D3D11_USAGE_DEFAULT,
				_In_ const UINT pCPUAccessFlags = 0,
				_In_ const UINT pMiscFlags = 0,
				_In_ const UINT pStructureByteStride = 0);
			//create pixel shader
			DX_EXP HRESULT create_index_buffer(_In_ ID3D11Device1* pDevice,
				_In_ const unsigned short* const pIndices,
				_In_ const UINT pIndicesSize,
				_In_ D3D11_USAGE pUsageFlag = D3D11_USAGE_DEFAULT,
				_In_ const UINT pCPUAccessFlags = 0,
				_In_ const UINT pMiscFlags = 0,
				_In_ const UINT pStructureByteStride = 0,
				_In_ const DXGI_FORMAT pFormat = DXGI_FORMAT_R16_UINT);
			//create default texture and use it
			DX_EXP HRESULT create_default_texture_2D(_In_ ID3D11Device1* pDevice);

			//initialize a texture and assign to material of mesh
			DX_EXP HRESULT create_texture_2D(_In_ ID3D11Device1* pDevice, 
				_In_ UINT pWidth, 
				_In_ UINT pHeight,
				_In_ D2D1::ColorF pColor,
				_In_ D3D11_USAGE pUsage = D3D11_USAGE_DEFAULT, 
				_In_ UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlag = D3D11_BIND_SHADER_RESOURCE);

			//initialize a texture and assign to material of mesh
			DX_EXP HRESULT create_texture_2D(_In_ ID3D11Device1* pDevice,
				_In_ UINT pWidth,
				_In_ UINT pHeight,
				_In_ D3D11_USAGE pUsage = D3D11_USAGE_DEFAULT,
				_In_ UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlag = D3D11_BIND_SHADER_RESOURCE,
				_In_ D3D11_SUBRESOURCE_DATA* pInitialData = nullptr);

			//Load Texture2D from path and assign to material of mesh
			DX_EXP HRESULT load_texture_2D_from_file(_In_ ID3D11Device1* pDevice, _In_z_ const std::wstring pPath, _In_ bool pIsAbsolutePath);

			//Render mesh with shader resource view of texture
			DX_EXP void render(_In_ ID3D11DeviceContext1* pContext, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//Render mesh with custom shader resource views
			DX_EXP void render(_In_ ID3D11DeviceContext1* pContext, _In_ std::vector<ID3D11ShaderResourceView*> pShaderResourceViews, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			//Update vertex buffer data
			DX_EXP HRESULT update_vertex_buffer_data(_In_ ID3D11DeviceContext1* pContext, _In_ void* pData, _In_ size_t pSizeOfDataInBytes);
			//update constant buffer of shader
			DX_EXP void update_shader_constant_buffer(_In_ ID3D11DeviceContext1* pContext, UINT pStartSlot, UINT pNumBuffer, _In_ ID3D11Buffer* pCBuffer);
			//update resource view of shader
			DX_EXP void update_shader_resource_view(_In_ ID3D11DeviceContext1* pContext, UINT pStartSLot, UINT pNumViews, _In_ ID3D11ShaderResourceView* pSRV);

			//Release all resources
			DX_EXP virtual ULONG release() override;

#pragma region Getters

			//Set position of mesh
			DX_EXP	glm::vec3					get_position() const; 
			//Get Texture2D of w_texture
			DX_EXP	ID3D11Texture2D*			get_texture_2D() const;
			//Get resource of w_texture
			DX_EXP	ID3D11Resource*				get_texture_resource() const;
			//Get shader resource view of w_texture
			DX_EXP	ID3D11ShaderResourceView*	get_shader_resource_view() const;
			//Get texture decription of w_texture
			DX_EXP	D3D11_TEXTURE2D_DESC		get_texture_2D_description() const;
			//Select active pixel shader
			DX_EXP	size_t						get_active_pixel_shader() const												{ return this->_active_pixel_shader; }

#pragma endregion

#pragma region Setters
		
			//Set position of mesh
			DX_EXP	void						set_position(_In_ const float pX, _In_ const float pY, _In_ const float pZ) { this->_position.x = pX; this->_position.y = pY; this->_position.z = pZ; }
			//Set position of mesh
			DX_EXP	void						set_position(_In_ const glm::vec3& pValue)									{ this->_position.x = pValue.x; this->_position.y = pValue.y; this->_position.z = pValue.z; }
			//Set position of mesh
			DX_EXP	void						set_position(_In_ const DirectX::XMVECTOR& pValue)							{ this->_position.x = DirectX::XMVectorGetX(pValue); this->_position.y = DirectX::XMVectorGetY(pValue); this->_position.z = DirectX::XMVectorGetZ(pValue); }
			//Set position of mesh
			DX_EXP	void						set_position(_In_ const DirectX::XMFLOAT3 pValue)							{ this->_position = pValue; }

			//Set rotation of mesh
			DX_EXP	void						set_rotation(_In_ const float pX, _In_ const float pY, _In_ const float pZ)	{ this->_rotation.x = pX; this->_rotation.y = pY; this->_rotation.z = pZ; }
			//Set rotation of mesh
			DX_EXP	void						set_rotation(_In_ const glm::vec3& pValue)									{ this->_rotation.x = pValue.x; this->_rotation.y = pValue.y; this->_rotation.z = pValue.z; }
			//Set rotation of mesh
			DX_EXP	void						set_rotation(_In_ const DirectX::XMVECTOR& pValue)							{ this->_rotation.x = DirectX::XMVectorGetX(pValue); this->_rotation.y = DirectX::XMVectorGetY(pValue); this->_rotation.z = DirectX::XMVectorGetZ(pValue); }
			//Set rotation of mesh
			DX_EXP	void						set_rotation(_In_ const DirectX::XMFLOAT3 pValue)							{ this->_rotation = pValue; }
			
			//Set scale of mesh	
			DX_EXP	void						set_scale(_In_ const float pX, _In_ const float pY, _In_ const float pZ)	{ this->_scale.x = pX; this->_scale.y = pY; this->_scale.z = pZ; }
			//Set scale of mesh
			DX_EXP	void						set_scale(_In_ const glm::vec3& pValue)										{ this->_scale.x = pValue.x; this->_scale.y = pValue.y; this->_scale.z = pValue.z; }
			//Set scale of mesh
			DX_EXP	void						set_scale(_In_ const DirectX::XMVECTOR& pValue)								{ this->_scale.x = DirectX::XMVectorGetX(pValue); this->_scale.y = DirectX::XMVectorGetY(pValue); this->_scale.z = DirectX::XMVectorGetZ(pValue); }
			//Set scale of mesh
			DX_EXP	void						set_scale(_In_ const DirectX::XMFLOAT3 pValue)								{ this->_scale = pValue; }

			//Set result of view * projection matrix
			DX_EXP	void						set_view_projection(_In_ glm::mat4x4& pValue)								{ this->_view_projection = DirectX::XMMatrixFromGLMMatrix(pValue); }
			//Set result of view * projection matrix
			DX_EXP	void						set_view_projection(_In_ DirectX::XMMATRIX pValue)							{ this->_view_projection = pValue; }

			//Select active pixel shader
			DX_EXP	void						set_active_pixel_shader(_In_ const size_t pIndex)							{ this->_active_pixel_shader = pIndex; }
			
			//Set fade amount value
//			DX_EXP	void				set_fade_value(float pValue)												{ this->_quad_cBuffer.data.fade_value = pValue; }
			//Set fade smooth value
//			DX_EXP	void				set_fade_smooth_value(float pValue)											{ this->_quad_cBuffer.data.fade_smooth = pValue; }
			////Set current frame of sprite
			//DX_EXP	void				set_sprite_current_frame(UINT pCurrentFrame)								{ this->_sprite_obj_cBuffer.data.CurrentFrame = static_cast<float>(pCurrentFrame); }
			////Set number of sprites	per row
			//DX_EXP	void				set_sprites_per_row(UINT pSpritesPerRow)									{ this->_sprite_obj_cBuffer.data.SpritesPerRow = static_cast<float>(pSpritesPerRow); }
			//Set number of sprites	per column
			//DX_EXP	void				set_sprites_per_column(UINT pSpritesPerColumn)								{ this->_sprite_obj_cBuffer.data.SpritesPerCol = static_cast<float>(pSpritesPerColumn); }
			//Enable/Disable using identity matrix
			DX_EXP	void						set_use_identity_matrix(bool pValue)										{ this->_use_identity_matrix = pValue; }
			
#pragma endregion
			
		private:
			typedef		system::w_object												_super;
			
			void		_update_world_view_projection(_In_ ID3D11DeviceContext1* pContext);

			//bool																		_is_sprite;
						
			bool																		_use_identity_matrix;
			DirectX::XMFLOAT3															_position;
			DirectX::XMFLOAT3															_rotation;
			DirectX::XMFLOAT3															_scale;
			DirectX::XMMATRIX															_world;
			DirectX::XMMATRIX															_view_projection;
			UINT																		_indices_size;
			UINT																		_stride;
			UINT																		_offset;

			w_constant_buffer<wvp_const_buffer>											_wvp_const_buffer;
			
			size_t																		_active_pixel_shader;
			std::unique_ptr<w_shader>													_shader;
			std::unique_ptr<w_texture_2D>												_texture;

			int																			_current_shader_index;
			int																			_current_texture_index;
			
			ID3D11Buffer*																_vertex_buffer;
			ID3D11Buffer*																_index_buffer;

			DXGI_FORMAT																	_index_buffer_format;
		};
	}
}

#endif