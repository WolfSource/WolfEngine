#include "Wolf.DirectX.Shared.PCH.h"
#include "W_RenderTarget2D.h"

using namespace Wolf::Graphics;
using namespace Microsoft::WRL;

W_RenderTarget2D::W_RenderTarget2D(const std::shared_ptr<W_GraphicsDevice>& pGDevice) : gDevice(pGDevice)
{
	this->Name = typeid(this).name();
}

W_RenderTarget2D::~W_RenderTarget2D()
{
	Release();
}

HRESULT W_RenderTarget2D::Load(int width, int height)
{
	auto device = this->gDevice->device;

	auto hr = S_FALSE;

	// Initialize the  texture description.
	// We will have our map be a square and also We will need to have this texture bound as a render target AND a shader resource
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	
	//Create the texture
	hr = device->CreateTexture2D(&textureDesc, NULL, &this->rt);
	OnFailed(hr, "Create texture2d for render target view", this->Name, false, true);

	//Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	//Map's Render Target
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	//Create the render target view.

	hr = device->CreateRenderTargetView(this->rt.Get(), &renderTargetViewDesc, &this->rtv);
	OnFailed(hr, "Create render target view for render target", this->Name, false, true);


	//Map's Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	hr = device->CreateShaderResourceView(this->rt.Get(), &shaderResourceViewDesc, &this->srv);
	OnFailed(hr, "Create Shader Resource View for render target", this->Name, false, true);

	ComPtr<ID3D11Texture2D> depthStencil = nullptr;
	{
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			width,
			height,
			1,
			1,
			D3D11_BIND_DEPTH_STENCIL);

		hr = this->gDevice->device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);
		OnFailed(hr, "creating depth stencil texture 2D", this->Name, true, true);

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		hr = this->gDevice->device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &this->depthStencilView);
		OnFailed(hr, "creating depth stencil view", this->Name, true, true);

		COM_RELEASE(depthStencil);
	}

	this->viewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		width,
		height);

	return hr;
}

void W_RenderTarget2D::Begin()
{
	const float backColor[4] = { 0.251f, 0.251f, 0.251f, 1.0f };
	auto context = this->gDevice->context;
	auto depth = this->depthStencilView.Get();

	//make sure to set the viewport for new width & height
	context->RSSetViewports(1, &this->viewport);
	context->OMSetRenderTargets(1, this->rtv.GetAddressOf(), depth);
	context->ClearRenderTargetView(this->rtv.Get(), backColor);
	context->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	depth = nullptr;
	context = nullptr;
}

void W_RenderTarget2D::End()
{
	ID3D11RenderTargetView* _rt = { NULL };
	auto depth = this->depthStencilView.Get();
	this->gDevice->context->OMSetRenderTargets(1, &_rt, depth);
}

ID3D11Resource* W_RenderTarget2D::GetResource()
{
	ID3D11Resource* resource = nullptr;
	this->srv->GetResource(&resource);
	return resource;
}

ULONG W_RenderTarget2D::Release()
{
	if (IsReleased()) return 0;

	COM_RELEASE(this->rtv);
	COM_RELEASE(this->rt);
	COM_RELEASE(this->depthStencilView);
	COM_RELEASE(this->srv);

	this->gDevice = nullptr;

	return W_Object::Release();
}



