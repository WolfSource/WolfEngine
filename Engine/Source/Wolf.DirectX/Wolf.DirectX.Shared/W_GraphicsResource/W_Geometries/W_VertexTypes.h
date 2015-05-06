/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_VertexTypes.h
	Description		 : The enumuration that is represent types of vertex
	Comment          :
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>

enum W_VertexDeclaration : byte
{
	NOP,
	Position,
	PositionColor,
	PositionNormalColor,
	PositionTexture,
	PositionNormalTexture,
	PositionNormalTextureTangent,
	PositionNormalTextureTangentBinormal,
	PositionNormalTangentColorTexture
};

namespace Wolf
{
	namespace Graphics
	{
		namespace VertexTypes
		{
			struct VertexPosition
			{
				DirectX::XMFLOAT3 Position;
			};

			struct VertexPositionColor
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT4 Color;
			};

			struct VertexPositionNormalColor
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Normal;
				DirectX::XMFLOAT4 Color;
			};

			struct VertexPositionTexture
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT2 TexCoord;
			};

			struct VertexPositionNormalTexture
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Normal;
				DirectX::XMFLOAT2 TexCoord;
			};

			struct VertexPositionNormalTextureTangent
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Normal;
				DirectX::XMFLOAT2 Texcoord;
				DirectX::XMFLOAT3 Tangent;
			};

			struct VertexPositionNormalTextureTangentBinormal
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Normal;
				DirectX::XMFLOAT2 Texcoord;
				DirectX::XMFLOAT3 Tangent;
				DirectX::XMFLOAT3 Binormal;
			};

			struct VertexPositionNormalTangentColorTexture
			{
				DirectX::XMFLOAT3 Position;
				DirectX::XMFLOAT3 Normal;
				DirectX::XMFLOAT4 Tangent;
				UINT     Color;
				DirectX::XMFLOAT2 TexCoord;
			};
		}
	}
}