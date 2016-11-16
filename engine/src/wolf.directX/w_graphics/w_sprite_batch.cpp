#include "w_directX_pch.h"
#include "w_sprite_batch.h"
#include <d2d1effects.h>
#include <w_math.h>
#include <w_graphics\w_textures\w_texture_2D.h>
#include <w_graphics\w_textures\ScreenGrab.h>

#define MISSING_BEGIN_CALL "Begin must be called successfully before a method can be called."
#define MISSING_END_CALL   "Begin cannot be called again until End has been successfully called."

using namespace Microsoft::WRL;
using namespace D2D1;
using namespace DirectX;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

w_sprite_batch::w_sprite_batch(const std::shared_ptr<w_graphics_device>& pGDevice) : _gDevice(pGDevice), _state(ENDED), _text(L"")
{
	_super::set_class_name(typeid(this).name());

	auto hr = _gDevice->factory_2D->CreateDrawingStateBlock(this->_state_block.GetAddressOf());
	V(hr, L"create drawing state block", this->name, 2, false, true);
}

w_sprite_batch::~w_sprite_batch()
{
	release();
}

void w_sprite_batch::load()
{
	//Load sprite font style
	this->_debug_sprite_font = std::make_unique<w_sprite_font>();
	this->_debug_sprite_font->load(this->_gDevice);

	//Create OffScreen texture
	auto _main_window = this->_gDevice->MainWindow();
	resize_render_target(_main_window->width, _main_window->height);

	add_effect(CLSID_D2D1BitmapSource);

	auto _size = this->_WIC_format_converters.size();
	if (_size)
	{
		std::for_each(this->_WIC_format_converters.begin(), this->_WIC_format_converters.end(), [](auto& pIter)
		{
			COM_RELEASE(pIter.second);
		});
		this->_WIC_format_converters.clear();
	}
}

void w_sprite_batch::resize_render_target(int pWidth, int pHeight)
{
	//Release all related resources
	COMPTR_RELEASE(this->_render_target_texture_2D);
	COMPTR_RELEASE(this->_render_target_texture_srv);
	COMPTR_RELEASE(this->_render_target);

#pragma region Create offscreen texture 2D and srv

	CD3D11_TEXTURE2D_DESC _texture_desc(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		pWidth,//Width
		pHeight,//Height
		1,// MipLevels
		1,// ArraySize
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

	auto _hr = this->_gDevice->device->CreateTexture2D(&_texture_desc, nullptr, &this->_render_target_texture_2D);
	V(_hr, L"creating texture 2D", this->name, 3, true, true);

	CD3D11_SHADER_RESOURCE_VIEW_DESC _shader_resource_view_desc(this->_render_target_texture_2D.Get(), D3D11_SRV_DIMENSION_TEXTURE2D);
	this->_gDevice->device->CreateShaderResourceView(this->_render_target_texture_2D.Get(), &_shader_resource_view_desc, &this->_render_target_texture_srv);
	V(_hr, L"creating shader resource view", this->name, 3, true, true);

#pragma endregion

	float _dxgiDpiX, _dxgiDpiY;
	this->_gDevice->context_2D->GetDpi(&_dxgiDpiX, &_dxgiDpiY);

#pragma region Create direct2D bitmap2D from texture2D

	//Get the offscreen texture as texture surface
	ComPtr<IDXGISurface> _texture_surface;
	_hr = this->_render_target_texture_2D.As(&_texture_surface);
	V(_hr, L"getting texture surface for offscreen texture", this->name, 3, true, true);

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			_dxgiDpiX,
			_dxgiDpiY);
	_hr = this->_gDevice->context_2D->CreateBitmapFromDxgiSurface(_texture_surface.Get(), &bitmapProperties, &this->_render_target);
	V(_hr, L"create bitmap from DxgiSurface", this->name, 3, true, true);

#pragma endregion

}

HRESULT w_sprite_batch::add_image_from_file(_In_z_ const wchar_t* pPath)
{
	if (!wolf::system::io::get_is_file(pPath)) return S_FALSE;

	auto _path = std::wstring(pPath);

	auto _exist = this->_WIC_format_converters.find(_path);
	if (_exist != this->_WIC_format_converters.end())
	{
		//Already exists in wolf resources
		return S_OK;
	}

	ComPtr<IWICBitmapDecoder> _decoder;
	auto _hr = this->_gDevice->image_factory->CreateDecoderFromFilename(
		_path.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&_decoder);
	V(_hr, L"decoding image from following image path: " + _path, this->name, 2, false, true);
	if (_hr == S_FALSE) return _hr;

	ComPtr<IWICBitmapFrameDecode> _frame;
	_decoder->GetFrame(0, &_frame);
	V(_hr, L"getting frame from image for following image path: " + _path, this->name, 2, false, true);
	if (_hr == S_FALSE) return _hr;

	IWICFormatConverter* _WIC_format_converter;
	//Creating format converter
	this->_gDevice->image_factory->CreateFormatConverter(&_WIC_format_converter);
	V(_hr, L"creating image format converter for following image path: " + _path, this->name, 2, false, true);

	_WIC_format_converter->Initialize(
		_frame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom);
	V(_hr, L"initializing iwic for frame of following image path : " + _path, this->name, 2);
	if (_hr == S_FALSE) return _hr;

	this->_WIC_format_converters[_path] = _WIC_format_converter;

	COMPTR_RELEASE(_decoder);
	COMPTR_RELEASE(_frame);

	return _hr;
}

void w_sprite_batch::add_effect(_In_ REFCLSID pEffectId)
{
	HRESULT _hr = S_FALSE;

	if ( pEffectId == CLSID_D2D1BitmapSource && this->_directional_blur_effect == nullptr)
	{
		auto _hr = this->_gDevice->context_2D->CreateEffect(CLSID_D2D1BitmapSource, &this->_bitmap_source_effect);
		V(_hr, L"creating bitmap source effect", this->name, 2, false, true);
	}
	else if (pEffectId == CLSID_D2D1DirectionalBlur && this->_directional_blur_effect == nullptr)
	{
		//Add directional blur
		_hr = this->_gDevice->context_2D->CreateEffect(CLSID_D2D1DirectionalBlur, &this->_directional_blur_effect);
		V(_hr, L"creating directional blur effect", this->name, 2, false, true);
	}
	else if (pEffectId == CLSID_D2D1GaussianBlur && this->_gaussian_blur_effect == nullptr)
	{
		//Add guassian blur
		_hr = this->_gDevice->context_2D->CreateEffect(CLSID_D2D1GaussianBlur, &this->_gaussian_blur_effect);
		V(_hr, L"creating gaussian blur effect", this->name, 2, false, true);
	}
	else if (pEffectId == CLSID_D2D1Scale && this->_scale_effect == nullptr)
	{
		//Add scale 
		_hr = this->_gDevice->context_2D->CreateEffect(CLSID_D2D1Scale, &this->_scale_effect);
		V(_hr, L"creating scale effect", this->name, 2, false, true);
	}
}

void w_sprite_batch::begin(_In_ bool pSaveState)
{
	if (this->_state != ENDED) throw std::exception(MISSING_END_CALL);
	this->_state = STARTED;

	if (pSaveState)
	{
		//Save the drawing status
		this->_gDevice->context_2D->SaveDrawingState(this->_state_block.Get());
	}
	this->_gDevice->context_2D->SetTarget(this->_gDevice->target_2D.Get());

	//Begin draw
	this->_gDevice->context_2D->BeginDraw();
	//Set the transform matrix
	this->_gDevice->context_2D->SetTransform(Matrix3x2F::Identity());
}

void w_sprite_batch::begin(_In_ D2D1::ColorF pClearColor, _In_ bool pRenderToTarget, _In_ bool pSaveState)
{
	if (this->_state != ENDED) throw std::exception(MISSING_END_CALL);
	this->_state = STARTED;

	if (pSaveState)
	{
		//Save the drawing status
		this->_gDevice->context_2D->SaveDrawingState(this->_state_block.Get());
	}

	if (pRenderToTarget)
	{
		this->_gDevice->context_2D->SetTarget(this->_render_target.Get());
	}
	else
	{
		this->_gDevice->context_2D->SetTarget(this->_gDevice->target_2D.Get());
	}

	//Begin draw
	this->_gDevice->context_2D->BeginDraw();
	//Set the transform matrix
	this->_gDevice->context_2D->SetTransform(Matrix3x2F::Identity());
	//if the opacity is zero, don not clear the back color of Direct2D surface 
	this->_gDevice->context_2D->Clear(pClearColor);
}

HRESULT w_sprite_batch::draw(_In_ wolf::graphics::direct2D::Isprite_batch_drawable* pSpriteBatchDrawableComp)
{
	if (this->_state != STARTED) throw std::exception(MISSING_BEGIN_CALL);

	if (pSpriteBatchDrawableComp == nullptr) return S_FALSE;

	return pSpriteBatchDrawableComp->draw();
}

HRESULT w_sprite_batch::draw_string(_In_z_ const std::wstring& pText, const D2D1_RECT_F pRectangle,
	_In_ ID2D1SolidColorBrush* pBrush, _In_ w_sprite_font* pSpriteFont, const D2D1_DRAW_TEXT_OPTIONS pOptions, const DWRITE_MEASURING_MODE pMeasuringMode, D2D1::Matrix3x2F pWorld)
{
	if (this->_state != STARTED) throw std::exception(MISSING_BEGIN_CALL);
	if (pText.empty()) return S_OK;

	auto _sprite_fonr_ptr = pSpriteFont == nullptr ? this->_debug_sprite_font.get() : pSpriteFont;

	auto _brush = (pBrush == nullptr ? _sprite_fonr_ptr->get_brush() : pBrush);

	this->_gDevice->context_2D->SetTransform(&pWorld);
	this->_gDevice->context_2D->DrawTextW(
		pText.c_str(),
		pText.size(),
		_sprite_fonr_ptr->get_font(),
		pRectangle,
		_brush,
		pOptions,
		pMeasuringMode);

	return S_OK;
}

HRESULT w_sprite_batch::draw_string(_In_z_ const std::wstring& pText, _In_ const DirectX::XMFLOAT2 pPosition,
	_In_ ID2D1SolidColorBrush* const pBrush, _In_ const w_sprite_font* pSpriteFont, _In_ D2D1::Matrix3x2F pWorld)
{
	if (this->_state != STARTED) throw std::exception(MISSING_BEGIN_CALL);
	if (pText.empty()) return S_OK;

	auto hr = S_FALSE;
	auto _text_layout = this->_debug_sprite_font->get_text_layout();
	if (this->_text != pText)
	{
		this->_text = pText;
		
		hr = this->_gDevice->write_factory->CreateTextLayout(
			_text.c_str(),
			_text.size(),
			pSpriteFont == nullptr ? this->_debug_sprite_font->get_font() : pSpriteFont->get_font(),
			4096.0f,
			4096.0f,
			&_text_layout);
		V(hr, L"create text layout", this->name, 2, false, true);

		//Set the new text layout
		this->_debug_sprite_font->set_text_layout(_text_layout);
	}

	auto _brush = (pBrush == nullptr ? pSpriteFont->get_brush() : pBrush);
	this->_gDevice->context_2D->SetTransform(&pWorld);
	this->_gDevice->context_2D->DrawTextLayout(D2D1::Point2F(pPosition.x, pPosition.y), _text_layout, _brush);
	
	return hr;
}

HRESULT w_sprite_batch::draw_image(_In_z_ const wchar_t* pImageName,
	_In_ const DirectX::XMFLOAT2 pPosition,
	_In_ const DirectX::XMFLOAT2 pScale, _In_ const DirectX::XMFLOAT2 pScaleCenter,
	_In_ const float pRotationAngle, _In_ const DirectX::XMFLOAT2 pRotationCenter,
	_In_ const DirectX::XMFLOAT2 pTranslation)
{
	if (this->_state != STARTED) throw std::exception(MISSING_BEGIN_CALL);

	auto _name = std::wstring(pImageName);
	auto _iter = this->_WIC_format_converters.find(_name);
	if (_iter == this->_WIC_format_converters.end()) {
		
		logger.warning(L"Could not find WIC format converter of followig image : " + _name);
		return S_FALSE;
	}
	else
	{
		set_bitmap_effect_source(_iter->second);
	}


	D2D1_POINT_2F _center_scale;
	_center_scale.x = pScaleCenter.x;
	_center_scale.y = pScaleCenter.y;

	D2D1_POINT_2F _center_rotation;
	_center_rotation.x = pRotationCenter.x;
	_center_rotation.y = pRotationCenter.y;

	this->_gDevice->context_2D->SetTransform(Matrix3x2F::Scale(pScale.x, pScale.y, _center_scale) *
		Matrix3x2F::Rotation(pRotationAngle, _center_rotation) * Matrix3x2F::Translation(pTranslation.x, pTranslation.y));
	this->_gDevice->context_2D->DrawImage(this->_bitmap_source_effect.Get(), D2D1::Point2F(pPosition.x, pPosition.y));
}


HRESULT w_sprite_batch::draw_image(_In_z_ const wchar_t* pImageName,
	_In_ const DirectX::XMFLOAT2 pPosition,
	_In_ D2D1_RECT_F pRectangle,
	_In_ const DirectX::XMFLOAT2 pScale, _In_ const DirectX::XMFLOAT2 pScaleCenter,
	_In_ const float pRotationAngle, _In_ const DirectX::XMFLOAT2 pRotationCenter,
	_In_ const DirectX::XMFLOAT2 pTranslation)
{
	if (this->_state != STARTED) throw std::exception(MISSING_BEGIN_CALL);

	auto _name = std::wstring(pImageName);
	auto _iter = this->_WIC_format_converters.find(_name);
	if (_iter == this->_WIC_format_converters.end()) {

		logger.warning(L"Could not find WIC format converter of followig image : " + _name);
		return S_FALSE;
	}
	else
	{
		set_bitmap_effect_source(_iter->second);
	}

	D2D1_POINT_2F _center_scale;
	_center_scale.x = pScaleCenter.x;
	_center_scale.y = pScaleCenter.y;

	D2D1_POINT_2F _center_rotation;
	_center_rotation.x = pRotationCenter.x;
	_center_rotation.y = pRotationCenter.y;

	this->_gDevice->context_2D->SetTransform(Matrix3x2F::Scale(pScale.x, pScale.y, _center_scale) * 
		Matrix3x2F::Rotation(pRotationAngle, _center_rotation) * Matrix3x2F::Translation(pTranslation.x, pTranslation.y));
	this->_gDevice->context_2D->DrawImage(this->_gaussian_blur_effect.Get(), D2D1::Point2F(pPosition.x, pPosition.y), pRectangle);
}

HRESULT w_sprite_batch::end()
{
	auto hr = this->_gDevice->context_2D->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		V(hr, L"end draw of sprite batch", this->name, 2, false, true);
	}
	this->_gDevice->context_2D->RestoreDrawingState(this->_state_block.Get());
	this->_state = ENDED;

	return hr;
}

ID3D11Resource* w_sprite_batch::get_resource() const
{
	ID3D11Resource* _resource = nullptr;
	this->_render_target_texture_srv->GetResource(&_resource);
	return _resource;
}

ID3D11ShaderResourceView* w_sprite_batch::get_srv() const
{
	return this->_render_target_texture_srv.Get();
}

ULONG w_sprite_batch::release()
{
	if (_super::is_released()) return 0;

	this->_text.clear();

	COMPTR_RELEASE(this->_state_block);
	COMPTR_RELEASE(this->_directional_blur_effect);
	COMPTR_RELEASE(this->_gaussian_blur_effect);
	COMPTR_RELEASE(this->_bitmap_source_effect);

	COMPTR_RELEASE(this->_render_target);
	COMPTR_RELEASE(this->_render_target_texture_2D);
	COMPTR_RELEASE(this->_render_target_texture_srv);

	UNIQUE_RELEASE(this->_debug_sprite_font);

	//Release all WIC format converters
	auto _size = this->_WIC_format_converters.size();
	if (_size)
	{
		std::for_each(this->_WIC_format_converters.begin(), this->_WIC_format_converters.end(), [](auto& pIter)
		{
			COM_RELEASE(pIter.second);
		});
		this->_WIC_format_converters.clear();
	}
	
	this->_gDevice = nullptr;

	return _super::release();
}

#pragma region Setters

void w_sprite_batch::set_bitmap_effect_source(IWICFormatConverter* pWICFormatConverter)
{
	auto _hr = this->_bitmap_source_effect->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, pWICFormatConverter);
	V(_hr, L"set bitmap source effect for following image path: ", this->name, 2, false, true);

	/*this->_gaussian_blur_effect->SetInputEffect(0, this->_bitmap_source_effect.Get());*/
}

void w_sprite_batch::set_gaussian_blur_value(D2D1_GAUSSIANBLUR_PROP pEffectProperty, float pValue)
{
	auto _hr = this->_gaussian_blur_effect.Get()->SetValue(pEffectProperty, pValue);
	V(_hr, L"set gaussian blur value", this->name, 2, false, true);
}

void w_sprite_batch::set_scale_value(D2D1_SCALE_PROP pEffectProperty, float pValue)
{
	auto _hr = this->_scale_effect.Get()->SetValue(pEffectProperty, pValue);
	V(_hr, L"set gaussian blur value", this->name, 2, false, true);
}

#pragma endregion
