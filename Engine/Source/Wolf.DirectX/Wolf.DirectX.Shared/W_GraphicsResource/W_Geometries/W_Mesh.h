/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : Mesh.h
	Description		 : Render a mesh
	Comment          : 
*/
//TODO: move sprite class to another class. class Sprite : public Quad {

#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include "W_VertexTypes.h"
#include "W_GraphicsResource\W_Shaders\W_Shader.h"
#include "W_GraphicsResource\W_Shaders\W_CBuffer.h"
#include "W_GraphicsResource\W_Textures\W_Texture2D.h"

#pragma pack(push,1)
//Defenition of quad object constant buffer
struct QuadObjInfo
{
	DirectX::XMMATRIX	WorldViewProjection;
	DirectX::XMFLOAT2	UVScale;
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

namespace Wolf
{
	namespace Graphics
	{
		//Represents a 3D model mesh composed of multiple meshpart objects.
		class W_Mesh : public System::W_Object
		{
		public:
			W_Mesh(const std::shared_ptr<Graphics::W_GraphicsDevice>& pGDevice);
			virtual ~W_Mesh();

			//Load a Identity quad
			API HRESULT Load(std::wstring pVertexShaderPath, std::wstring pPixelShaderPath, const Wolf::Graphics::VertexTypes::VertexPositionNormalTexture* const pVertices, 
				const UINT pVerticesSize, const unsigned short* const pIndices, const UINT pIndicesSize);
			//initialize default texture and assign to material of mesh
			API HRESULT LoadDeafultTexture(UINT pWidth, UINT pHeight);
			//Load Texture2D from path and assign to material of mesh
			API HRESULT LoadTexture2D(std::wstring pPath);
			//Render mesh by using shader resource view
			API void Render(ID3D11ShaderResourceView* srv = nullptr);
			//Release all resources
			API virtual ULONG Release() override;

#pragma region Getters

			//Get whether mesh uses default texture 
			bool						GetUseDefaultTexture()							{ return this->useDefaultTexture; }
			//Get resource of Texture2D
			ID3D11Resource*				GetResource()									{ return this->texture2D->GetResource(); }
			//Get shader resource view of Texture2D
			ID3D11ShaderResourceView*	GetSRV()										{ return this->texture2D->GetSRV(); }

#pragma endregion

#pragma region Setters

			//Enable/Disable sptite animation
			void						SetIsSprite(bool pValue)						{ this->isSprite = pValue;  }
			//Set position of mesh
			API void					SetPosition(DirectX::XMFLOAT3 pValue)			{ this->position = pValue; }
			//Set rotation of mesh
			API void					SetRotation(DirectX::XMFLOAT3 pValue)			{ this->rotation = pValue; }
			//Set scale of mesh
			API void					SetScale(DirectX::XMFLOAT3 pValue)				{ this->scale = pValue; }
			//Set view matrix
			API void					SetView(DirectX::XMMATRIX pValue)				{ this->view = pValue; }
			//Set projection matrix
			API void					SetProjection(DirectX::XMMATRIX pValue)			{ this->projection = pValue; }
			//Set uv
			API void					SetUVScale(DirectX::XMFLOAT2 pUVScale)			{ this->objInfoCBuffer.Const.UVScale = pUVScale; }
			//Set current frame of sprite
			API void					SetSpriteCurrentFrame(UINT pCurrentFrame)		{ this->spriteObjCBuffer.Const.CurrentFrame = static_cast<float>(pCurrentFrame); }
			//Set number of sprites per row
			API void					SetSpritesPerRow(UINT pSpritesPerRow)			{ this->spriteObjCBuffer.Const.SpritesPerRow = static_cast<float>(pSpritesPerRow); }
			//Set number of sprites per column
			API void					SetSpritesPerColumn(UINT pSpritesPerColumn)		{ this->spriteObjCBuffer.Const.SpritesPerCol = static_cast<float>(pSpritesPerColumn); }
			//Set use default texture
			API void					SetUseDefaultTexture(bool value)				{ this->useDefaultTexture = value; }
			//Enable/Disable using identity matrix
			API void					SetUseIdentityMatrix(bool value)				{ this->useIdentityMatrix = value; }
			
#pragma endregion

		protected:
			std::shared_ptr<Graphics::W_GraphicsDevice>									gDevice;

		private:
			bool																		isSprite;
			bool																		useIdentityMatrix;
			bool																		useDefaultTexture;
			DirectX::XMFLOAT3															position;
			DirectX::XMFLOAT3															rotation;
			DirectX::XMFLOAT3															scale;
			DirectX::XMMATRIX															world;
			DirectX::XMMATRIX															view;
			DirectX::XMMATRIX															projection;
			UINT																		indicesSize;
			UINT																		stride;
			UINT																		offset;

			W_CBuffer<QuadObjInfo>														objInfoCBuffer;
			W_CBuffer<SpriteObjInfo>													spriteObjCBuffer;

			std::unique_ptr<W_Shader>													shader;
			std::unique_ptr<W_Texture2D>												texture2D;
			std::unique_ptr<W_Texture2D>												defaultTexture;
			Microsoft::WRL::ComPtr<ID3D11Buffer>										vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>										indexBuffer;
		};
	}
}
