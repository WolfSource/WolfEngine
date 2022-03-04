#include "w_directX_pch.h"
#include "w_image.h"
#include "w_widget.h"

using namespace wolf::gui;

w_image::w_image(_In_opt_ w_widget* pParent) : _super(pParent),
path(L""),
is_absolute_path(false),
use_default_size(true),
rotation(0, 0),
_device(nullptr)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_IMAGE;

	_quad::set_use_identity_matrix(true);
}

w_image::~w_image()
{
	release();
}

HRESULT w_image::on_initialize(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	this->_device = pGDevice->device.Get();

	_quad::initialize(this->_device);

	if (!this->path.empty())
	{
		if (FAILED(_quad::load_texture_2D_from_file(this->_device, this->path, this->is_absolute_path)))
		{
			logger.warning(L"Could not load texture for w_image from following path:" + this->path);
		}
	}

	if (this->use_default_size)
	{
		auto _description = _quad::get_texture_2D_description();
		_super::width = _description.Width;
		_super::height = _description.Height;
	}

	//load const buffer
	_quad::const_buffer.load(this->_device);

	//create vertex buffer
	create_or_update_vertex_buffer(pGDevice, false);

	if (FAILED(_quad::create_vertex_shader(this->_device, L"Shaders\\quad_vs.cso")))  return S_FALSE;
	if (FAILED(_quad::create_pixel_shader(this->_device, L"Shaders\\quad_texture_ps.cso"))) return S_FALSE;

	return S_OK;
}

HRESULT w_image::create_or_update_vertex_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ bool pUpdate)
{
	int _parent_x, _parent_y;
	_super::parent_widget->get_position(_parent_x, _parent_y);
	auto _position = DirectX::XMFLOAT2(
		float(_parent_x + _super::x),
		float(_parent_y + _super::y));

	UINT _background_buffer_width = 512;
	UINT _background_buffer_height = 512;
	if (this->parent_widget)
	{
		_background_buffer_width = this->parent_widget->get_background_buffer_width();
		_background_buffer_height = this->parent_widget->get_background_buffer_height();
	}

	float _left = _position.x * 2.0f / _background_buffer_width - 1.0f;
	float _right = (_position.x + _super::width) * 2.0f / _background_buffer_width - 1.0f;
	float _top = 1.0f - _position.y * 2.0f / _background_buffer_height;
	float _bottom = 1.0f - (_position.y + _super::height) * 2.0f / _background_buffer_height;

	const UINT _vertices_size = 4;
	using namespace wolf::content_pipeline::vertex_declaration_structs;
	const vertex_position_normal_uv _vertices[_vertices_size] =
	{
		{ glm::vec3(_right,  _top    , 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(_left ,  _bottom , 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(_left ,  _top    , 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(_right,  _bottom , 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }
	};

	if (pUpdate)
	{
		return _quad::update_vertex_buffer_data(pGDevice->context.Get(), (void*)_vertices, sizeof(_vertices));
	}

	auto _device = pGDevice->device.Get();

	const UINT _indices_size = 6;
	const unsigned short _indices [] = { 0, 1, 2, 0, 3, 1 };

	if (FAILED(_quad::create_index_buffer(_device, _indices, _indices_size))) return S_FALSE;

	return _quad::create_vertex_buffer(_device,
		_vertices,
		_vertices_size,
		sizeof(vertex_position_normal_uv),
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE,
		0,
		0);
}

void w_image::render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!_super::visible) return;
	
	if (_super::redraw)
	{
		create_or_update_vertex_buffer(pGDevice, true);
		_super::redraw = false;
	}
	_quad::render(pGDevice->context.Get());
}

ULONG w_image::release()
{
	if (_super::get_is_released()) return 0;

	if (!_quad::get_is_released())
	{
		_quad::release();
	}
	_device = nullptr;
	
	return _super::release();
}

#pragma region Setters

HRESULT w_image::set_path(_In_z_ const std::wstring& pValue, _In_ const bool pIsAbsolutePath)
{
	if (pValue.empty()) return S_FALSE;

	this->path = pValue;
	this->is_absolute_path = pIsAbsolutePath;

	if (!this->_device)
	{
		return S_FALSE;
	}
	return _quad::load_texture_2D_from_file(this->_device, this->path, this->is_absolute_path);
}

void w_image::set_opacity(_In_ const float pValue)
{
	_quad::const_buffer.data.opacity = pValue;
	_super::redraw = true;
}

void w_image::set_uv_scale(_In_ const glm::vec2 pValue)
{
	_quad::const_buffer.data.uv_scale.x = pValue.x;
	_quad::const_buffer.data.uv_scale.y = pValue.y;
	_super::redraw = true;
}

#pragma endregion

#pragma region Getters

float w_image::get_opacity() const
{
	return _quad::const_buffer.data.opacity;
}

glm::vec2 w_image::get_uv_scale() const
{
	glm::vec2 _v;
	_v.x = _quad::const_buffer.data.uv_scale.x;
	_v.y = _quad::const_buffer.data.uv_scale.y;
	
	return _v;
}

#pragma endregion