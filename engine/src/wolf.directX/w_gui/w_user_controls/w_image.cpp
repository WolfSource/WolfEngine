#include "w_directX_pch.h"
#include "w_image.h"
#include "w_widget.h"

using namespace wolf::gui;

w_image::w_image(_In_opt_ w_widget* pParent) : _super(pParent),
path(L""),
scale(1, 1),
rotation(0, 0),
_device(nullptr),
_mesh(nullptr)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_IMAGE;

	this->_mesh = new wolf::graphics::w_mesh();
	this->_mesh->set_use_identity_matrix(true);
}

w_image::~w_image()
{
	release();
}

HRESULT w_image::on_initialize(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	this->_device = pGDevice->device.Get();

	this->_mesh->initialize(this->_device);

	//if (FAILED(this->_mesh->create_and_use_default_texture_2d(_device)))  return S_FALSE;
	if (!this->path.empty())
	{
		if (FAILED(this->_mesh->load_texture_2D_from_file(this->_device, this->path, true)))
		{
			logger.warning(L"Could not load texture for w_image from following path:" + this->path);
		}
	}

	auto _description = this->_mesh->get_texture_2D_description();
	_super::width = _description.Width;
	_super::height = _description.Height;
	
	create_or_update_vertex_buffer(pGDevice, false);

	if (FAILED(this->_mesh->create_vertex_shader(this->_device, L"Shaders\\quad_vs.cso")))  return S_FALSE;
	if (FAILED(this->_mesh->create_pixel_shader(this->_device, L"Shaders\\quad_ps.cso"))) return S_FALSE;

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
	float _right = (_position.x + this->width) * 2.0f / _background_buffer_width - 1.0f;
	float _top = 1.0f - _position.y * 2.0f / _background_buffer_height;
	float _bottom = 1.0f - (_position.y + this->height) * 2.0f / _background_buffer_height;

	const UINT _vertices_size = 4;
	using namespace wolf::content_pipeline::vertex_declaration_structs;
	const vertex_position_normal_uv _vertices[_vertices_size] =
	{
		{ glm::vec3(_right * this->scale.x,  _top    * this->scale.y, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(_left  * this->scale.x,  _bottom * this->scale.y, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(_left  * this->scale.x,  _top    * this->scale.y, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(_right * this->scale.x,  _bottom * this->scale.y, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }
	};

	if (pUpdate)
	{
		return this->_mesh->update_vertex_buffer_data(pGDevice->context.Get(), (void*)_vertices, sizeof(_vertices));
	}

	auto _device = pGDevice->device.Get();

	const UINT _indices_size = 6;
	const unsigned short _indices [] = { 0, 1, 2, 0, 3, 1 };

	if (FAILED(this->_mesh->create_index_buffer(_device, _indices, _indices_size))) return S_FALSE;

	return this->_mesh->create_vertex_buffer(_device,
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
	if (_super::visible == false || !this->_mesh) return;
	
	if (_super::redraw)
	{
		create_or_update_vertex_buffer(pGDevice, true);
		_super::redraw = false;
	}
	this->_mesh->render(pGDevice->context.Get());
}

ULONG w_image::release()
{
	if (this->is_released()) return 0;

	SAFE_RELEASE(this->_mesh);
	_device = nullptr;
	
	return _super::release();
}

#pragma region Setters

HRESULT w_image::set_path(_In_z_ const std::wstring& pValue)
{
	if (pValue.empty()) return S_FALSE;

	this->path = pValue;

	if (!this->_device)
	{
		return S_FALSE;
	}
	return this->_mesh->load_texture_2D_from_file(this->_device, this->path, true);
}

#pragma endregion
