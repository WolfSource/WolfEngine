#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Mesh.h"
#include "W_GraphicsResource\W_Textures\W_RenderTarget2D.h"

using namespace std;
using namespace D2D1;
using namespace DirectX;
using namespace Wolf::System;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::VertexTypes;

W_Mesh::W_Mesh(const shared_ptr<W_GraphicsDevice>& pGDevice) : gDevice(pGDevice), stride(sizeof(VertexPositionNormalTexture)), 
	offset(0), useIdentityMatrix(false), isSprite(false), texture2DWidth(512), texture2DHeight(512)
{
	this->Name = typeid(this).name();

	this->texture2D = make_unique<W_Texture2D>();
	this->shader = make_unique<W_Shader>(pGDevice);
	this->objInfoCBuffer.Const.UVScale = XMFLOAT2(1, 1);
	this->position = XMFLOAT3(0, 0, 0);
	this->rotation = XMFLOAT3(0, 0, 0);
	this->scale = XMFLOAT3(1, 1, 1);
}

W_Mesh::~W_Mesh()
{
	Release();
}

HRESULT W_Mesh::Load(std::wstring pVertexShaderPath, std::wstring pPixelShaderPath, std::wstring pTexture2DPath, bool pLoadDeafultTexture2D,
	const VertexPositionNormalTexture* const pVertices, const UINT pVerticesSize, const unsigned short* const pIndices, const UINT pIndicesSize)
{
	this->indicesSize = pIndicesSize;

#pragma region Create Vertex Buffer

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vBufferDesc.ByteWidth = sizeof(VertexPositionNormalTexture) * pVerticesSize;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData;
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = pVertices;

	auto hr = this->gDevice->device->CreateBuffer(&vBufferDesc, &vBufferData, &this->vertexBuffer);
	OnFailed(hr, "creating vertex buffer", this->Name, true, true);

#pragma endregion

#pragma region Create Index Buffer

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * this->indicesSize;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = pIndices;

	hr = this->gDevice->device->CreateBuffer(&indexBufferDesc, &indexBufferData, &this->indexBuffer);
	OnFailed(hr, "creating index buffer", this->Name, true, true);

#pragma endregion

	//Load constant buffer for each section
	if (isSprite)
	{
		this->spriteObjCBuffer.Load(this->gDevice);
		//Set the initial constants
		this->spriteObjCBuffer.Const.CurrentFrame = 0;
		this->spriteObjCBuffer.Const.SpritesPerRow = 1;
		this->spriteObjCBuffer.Const.SpritesPerCol = 1;
	}
	else
	{
		this->objInfoCBuffer.Load(this->gDevice);
	}

	//If we need to load default texture load it, on the other hand load it from storage
	if (pTexture2DPath == L"")
	{
		if (pLoadDeafultTexture2D)
		{
			LoadDeafultTexture(this->texture2DWidth, this->texture2DHeight);
		}
	}
	else
	{
		LoadTexture2D(pTexture2DPath);
	}

	//Load shaders
	hr = W_Shader::LoadShader(pVertexShaderPath, W_ShaderType::VertexShader, W_VertexDeclaration::PositionNormalTexture, this->shader.get());
	OnFailed(hr, "loading vertex shader", this->Name, true, true);

	hr = W_Shader::LoadShader(pPixelShaderPath, W_ShaderType::PixelShader, W_VertexDeclaration::NOP, this->shader.get());
	OnFailed(hr, "loading pixel shader", this->Name, true, true);

	//Create world matrix
	auto posVect = XMLoadFloat3(&this->position);
	auto rotVect = XMLoadFloat3(&this->rotation);

	this->world = XMMatrixScaling(scale.x, scale.y, scale.z) *
		XMMatrixRotationRollPitchYawFromVector(rotVect) *
		XMMatrixTranslationFromVector(posVect);

	return hr;
}

HRESULT W_Mesh::LoadDeafultTexture(UINT pWidth, UINT pHeight, D3D11_USAGE pUsage, D3D11_CPU_ACCESS_FLAG pCpuAccess)
{
	auto device = this->gDevice->device.Get();
	//Create default texture
	auto hr = this->texture2D->Create(device, pWidth, pHeight, pUsage, pCpuAccess);
	OnFailed(hr, "Creating default texture", this->Name, true, true);
	return hr;
}

HRESULT W_Mesh::LoadTexture2D(wstring pPath)
{
	auto device = this->gDevice->device.Get();
	auto hr = W_Texture2D::LoadTexture(device, this->texture2D.get(), pPath);
	OnFailed(hr, "loading texture", this->Name, false, true);
	return hr;
}

void W_Mesh::Render(ID3D11ShaderResourceView* srv)
{
	auto context = this->gDevice->context;
	{
		//Set worldViewProjection, we need to update constant buffers for each frame
		auto window = this->gDevice->MainWindow();
		if (window == nullptr) OnFailed(S_FALSE, "getting main window", this->Name);

		auto posVect = XMLoadFloat3(&this->position);
		auto rotVect = XMLoadFloat3(&this->rotation);
		this->world = XMMatrixScaling(scale.x, scale.y, scale.z) *
			XMMatrixRotationRollPitchYawFromVector(rotVect) *
			XMMatrixTranslationFromVector(posVect);

		auto WVP = XMMatrixTranspose(this->world * this->view * this->projection);
		if (this->isSprite)
		{
			this->spriteObjCBuffer.Const.WorldViewProjection = useIdentityMatrix ? XMMatrixIdentity() : WVP;
			this->spriteObjCBuffer.Update();
			this->shader->SetConstantBuffer(0, 1, this->spriteObjCBuffer.GetBuffer());
		}
		else
		{
			this->objInfoCBuffer.Const.WorldViewProjection = useIdentityMatrix ? XMMatrixIdentity() : WVP;
			this->objInfoCBuffer.Update();

			this->shader->SetConstantBuffer(0, 1, this->objInfoCBuffer.GetBuffer());
		}

		//Set shader resource view
		this->shader->SetSRV(0, 1, srv != nullptr ? srv : this->texture2D->GetSRV());
		
		this->shader->Apply();
		{
			context->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &this->stride, &this->offset);
			context->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->DrawIndexed(this->indicesSize, 0, 0);
		}
	}

	context = nullptr;
}

ULONG W_Mesh::Release()
{
	if (this->IsReleased()) return 0;
	
	COM_RELEASE(this->vertexBuffer);
	COM_RELEASE(this->indexBuffer);
	UNIQUE_RELEASE(this->texture2D);
	UNIQUE_RELEASE(this->shader);

	//Release shader's constant buffers
	this->objInfoCBuffer.Release();
	this->spriteObjCBuffer.Release();

	return 1;
}