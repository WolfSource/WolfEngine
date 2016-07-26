#include "w_directX_pch.h"
#include "w_render_target_2D.h"

using namespace wolf::graphics;
using namespace Microsoft::WRL;

w_render_target_2D::w_render_target_2D()
{
	_super::set_class_name(typeid(this).name());
}

w_render_target_2D::~w_render_target_2D()
{
	release();
}

HRESULT w_render_target_2D::load(_In_ ID3D11Device1* pDevice, int width, int height)
{
	auto _hr = S_FALSE;

	// Initialize the  texture description.
	// We will have our map be a square and also We will need to have this texture bound as a render target AND a shader resource
	D3D11_TEXTURE2D_DESC _textureDesc;
	ZeroMemory(&_textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	_textureDesc.Width = width;
	_textureDesc.Height = height;
	_textureDesc.MipLevels = 1;
	_textureDesc.ArraySize = 1;
	_textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	_textureDesc.SampleDesc.Count = 1;
	_textureDesc.SampleDesc.Quality = 0;
	_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	_textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	_textureDesc.CPUAccessFlags = 0;
	_textureDesc.MiscFlags = 0;
	
	//Create the texture
	_hr = pDevice->CreateTexture2D(&_textureDesc, NULL, &this->_rt);
	V(_hr, L"create texture2d for render target view", this->name, 2, false, true);

	//Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC _renderTargetViewDesc;
	ZeroMemory(&_renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	//Map's Render Target
	_renderTargetViewDesc.Format = _textureDesc.Format;
	_renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	_renderTargetViewDesc.Texture2D.MipSlice = 0;
	//Create the render target view.

	_hr = pDevice->CreateRenderTargetView(this->_rt.Get(), &_renderTargetViewDesc, &this->_rtv);
	V(_hr, L"create render target view for render target", this->name, 2, false, true);
	
	//Map's Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC _shaderResourceViewDesc;
	ZeroMemory(&_shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	_shaderResourceViewDesc.Format = _textureDesc.Format;
	_shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	_shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	_shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	_hr = pDevice->CreateShaderResourceView(this->_rt.Get(), &_shaderResourceViewDesc, &this->_srv);
	V(_hr, L"create Shader Resource View for render target", this->name, 2, false, true);

	ComPtr<ID3D11Texture2D> _depthStencil = nullptr;
	{
		CD3D11_TEXTURE2D_DESC _depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			width,
			height,
			1,
			1,
			D3D11_BIND_DEPTH_STENCIL);

		_hr = pDevice->CreateTexture2D(&_depthStencilDesc, nullptr, &_depthStencil);
		V(_hr, L"creating depth stencil texture 2D", this->name, 3, true, true);

		CD3D11_DEPTH_STENCIL_VIEW_DESC _depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		_hr = pDevice->CreateDepthStencilView(_depthStencil.Get(), &_depthStencilViewDesc, &this->_depthStencilView);
		V(_hr, L"creating depth stencil view", this->name, 3, true, true);

		COMPTR_RELEASE(_depthStencil);
	}

	this->_viewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		width,
		height);

	return _hr;
}

void w_render_target_2D::begin(_In_ ID3D11DeviceContext1* pContext, DirectX::XMFLOAT4 pClearColor)
{
	const float backColor[4] = { pClearColor.x, pClearColor.y, pClearColor.z, pClearColor.w };
	auto depth = this->_depthStencilView.Get();

	//make sure to set the viewport for new width & height
	pContext->RSSetViewports(1, &this->_viewport);
	pContext->OMSetRenderTargets(1, this->_rtv.GetAddressOf(), depth);
	pContext->ClearRenderTargetView(this->_rtv.Get(), backColor);
	pContext->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	depth = nullptr;
}

void w_render_target_2D::end(_In_ ID3D11DeviceContext1* pContext)
{
	ID3D11RenderTargetView* _rt = { NULL };
	auto _depth = this->_depthStencilView.Get();
	pContext->OMSetRenderTargets(1, &_rt, _depth);
}

ID3D11Resource* w_render_target_2D::get_resource()
{
	ID3D11Resource* _resource = nullptr;
	this->_srv->GetResource(&_resource);
	return _resource;
}

ULONG w_render_target_2D::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_rtv);
	COMPTR_RELEASE(this->_rt);
	COMPTR_RELEASE(this->_depthStencilView);
	COMPTR_RELEASE(this->_srv);
	
	return _super::release();
}



