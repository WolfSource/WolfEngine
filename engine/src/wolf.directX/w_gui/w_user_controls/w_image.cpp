#include "w_directX_pch.h"
#include "w_image.h"
#include "w_widget.h"

using namespace wolf::gui;

w_image::w_image(_In_opt_ w_widget* pParent) : _super(pParent),
_path(L""),
_scale(1, 1),
_rotation_angle(0),
_rotation_center(0, 0),
_translation(0, 0)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_IMAGE;
}

void w_image::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (_super::visible == false) return;

	W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;

	if (!_super::enabled)
	{
		_state = W_GUI_STATE_DISABLED;
	}

	//position
	UINT _x, _y;
	_super::parent_widget->get_position(_x, _y);
	auto _position = DirectX::XMFLOAT2(float(_x + _super::x + (_super::width / 2)), float(_y + _super::y + (_super::height / 2)));
	//Scale base on the position of image
	auto _scale_center = _position;

	this->_crop.left = 0;
	this->_crop.top = 0;
	this->_crop.right = static_cast<float>(this->width);
	this->_crop.bottom = static_cast<float>(this->height);
	if ((this->_crop.right - _crop.left) != 0 && (this->_crop.bottom - this->_crop.top) != 0)
	{
		_super::parent_widget->draw_image(this->_path.c_str(),
			_position, this->_crop,
			this->_scale, _scale_center,
			this->_rotation_angle, this->_rotation_center,
			this->_translation,
			D2D1_GAUSSIANBLUR_PROP::D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, this->_guasian_effect_value);
	}
	else
	{
		_super::parent_widget->draw_image(this->_path.c_str(),
			_position,
			this->_scale, _scale_center,
			this->_rotation_angle, this->_rotation_center,
			this->_translation,
			D2D1_GAUSSIANBLUR_PROP::D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, this->_guasian_effect_value);
	}
}


