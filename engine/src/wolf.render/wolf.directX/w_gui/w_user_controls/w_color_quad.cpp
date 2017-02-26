#include "w_directX_pch.h"
#include "w_color_quad.h"
#include "w_widget.h"

using namespace wolf::gui;

w_color_quad::w_color_quad(_In_opt_ w_widget* pParent) : _super(pParent),
rotation(0, 0),
_device(nullptr)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_COLOR_QUAD;

	_quad::set_use_identity_matrix(true);
}

w_color_quad::~w_color_quad()
{
	release();
}

HRESULT w_color_quad::on_initialize(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	this->_device = pGDevice->device.Get();

	_quad::initialize(this->_device);

	//load const buffer
	_quad::const_buffer.load(this->_device);

	//create vertex buffer
	create_or_update_vertex_buffer(pGDevice, false);

	if (FAILED(_quad::create_vertex_shader(this->_device, L"Shaders\\quad_vs.cso")))  return S_FALSE;
	if (FAILED(_quad::create_pixel_shader(this->_device, L"Shaders\\quad_color_ps.cso"))) return S_FALSE;

	return S_OK;
}

HRESULT w_color_quad::create_or_update_vertex_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ bool pUpdate)
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

void w_color_quad::render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!_super::visible) return;
	
	if (_super::redraw)
	{
		create_or_update_vertex_buffer(pGDevice, true);
		_super::redraw = false;
	}
	_quad::render(pGDevice->context.Get());
}

ULONG w_color_quad::release()
{
	if (_super::get_is_released()) return 0;

	if (!_quad::get_is_released())
	{
		_quad::release();
	}
	_device = nullptr;
	
	return _super::release();
}

#pragma region Getter

const w_color w_color_quad::get_color() const
{
	w_color _color;
	_color.r = _quad::const_buffer.data.color.x * 255.0f;
	_color.g = _quad::const_buffer.data.color.y * 255.0f;
	_color.b = _quad::const_buffer.data.color.z * 255.0f;
	_color.a = _quad::const_buffer.data.color.w * 255.0f;

	return _color;
}

#pragma endregion

#pragma region Setter

void w_color_quad::set_color(_In_ const w_color pValue)
{
	_quad::const_buffer.data.color.x = pValue.r / 255.0f;
	_quad::const_buffer.data.color.y = pValue.g / 255.0f;
	_quad::const_buffer.data.color.z = pValue.b / 255.0f;
	_quad::const_buffer.data.color.w = pValue.a / 255.0f;
}

#pragma endregion
