#include "w_directX_pch.h"
#include "w_gui.h"
#include <w_xml.h>

using namespace wolf::gui;
using namespace wolf::graphics;

std::once_flag												_once_flag;
bool														w_gui::_is_released = false;
std::shared_ptr<w_graphics_device>							w_gui::_gDevice;
w_sprite_batch*												w_gui::_sprite_batch;
wolf::gui::w_widgets_resource_manager*						w_gui::_widget_resource_manager = nullptr;
std::map<const std::string, w_widget*>						w_gui::_widgets;
w_widget*													w_gui::_temp_parent_widget_ptr = nullptr;
UINT														w_gui::_parent_width;
UINT														w_gui::_parent_height;
const char*													w_gui::_trace_class_name = "w_gui";

#ifdef WIN32
HWND														w_gui::_hwnd = NULL;
#endif

void w_gui::initialize(const std::shared_ptr<w_graphics_device>& pGDevice, w_sprite_batch* pSpriteBatch, UINT pWidth, UINT pHeight)
{
	_is_released = false;
	
	_parent_width = pWidth;
	_parent_height = pHeight;

	//store gDevice once
	std::call_once(_once_flag, [pGDevice, pSpriteBatch, pWidth, pHeight]()
	{
		_gDevice = pGDevice;
		_sprite_batch = pSpriteBatch;

		//create widget manager
		_widget_resource_manager = new wolf::gui::w_widgets_resource_manager();
		_widget_resource_manager->on_device_created(pGDevice->device.Get(), pGDevice->context.Get());
		_widget_resource_manager->on_swapChain_resized(pWidth, pHeight);
	});
}

HRESULT w_gui::load(HWND pHwnd, const wchar_t* pGuiDesignPath)
{
	_hwnd = pHwnd;

	//parse gui from xml file
	if (wolf::system::io::get_is_file(pGuiDesignPath) == S_FALSE)
	{
		wchar_t _msg[1024];
		wsprintf(_msg, L"load wolf.gui design file, file not found. %s", pGuiDesignPath);
		V(S_FALSE, _msg, _trace_class_name, 3);
		return S_FALSE;
	}

	//first open the stream of xml file 
	std::ifstream _file(pGuiDesignPath);
	std::stringstream _string_stream;
	_string_stream << _file.rdbuf();
	//close file
	_file.close();
	//read all stream into the content
	std::string _content(_string_stream.str());
	//clear string stream
	_string_stream.clear();

	using namespace rapidxml;
	xml_document<> _doc;
	bool _error = false;
	try
	{
		_doc.parse<0>(&_content[0]);
	}
	catch (...)
	{
		_error = true;

		wchar_t _msg[2048];
		wsprintf(_msg, L"wolf.gui design file, file corrupted: %s", pGuiDesignPath);
		V(S_FALSE, _msg, _trace_class_name, 3);
	}

	if (_error) return S_FALSE;

	//read the first node
	auto _wolf_gui_node = _doc.first_node();
	if (!_wolf_gui_node)
	{
		wchar_t _msg[2048];
		wsprintf(_msg, L"wolf.gui design file, it might be is empty or corrupted. %s", pGuiDesignPath);
		V(S_FALSE, _msg, _trace_class_name, 3);
		return S_FALSE;
	}

	auto _name = std::string(_wolf_gui_node->name());
	std::transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
	if (_name != "wolf.gui")
	{
		wchar_t _msg[2048];
		wsprintf(_msg, L"wolf.gui design file, the design file must be started with <wolf.gui>...</wolf.gui>.: %s", pGuiDesignPath);
		V(S_FALSE, _msg, _trace_class_name, 3);
		return S_FALSE;
	}

	//Check all nodes inside the wolf.gui
	for (auto _node = _wolf_gui_node->first_node(); _node != nullptr; _node = _node->next_sibling())
	{
		_traversing_gui_node(_node, pHwnd, pGuiDesignPath);
	}

	_doc.clear();

	return S_OK;
}

void w_gui::_traversing_gui_node(rapidxml::xml_node<char>* pNode, HWND pHwnd, const wchar_t* pGuiDesignPath)
{
	if (!pNode) return;

	//get neame
	auto _name = std::string(pNode->name());
	std::transform(_name.begin(), _name.end(), _name.begin(), ::tolower);

	if (_name == "widget")
	{
#pragma region create widget from xml

		auto _name = wolf::system::w_xml::get_node_attribute(pNode, "Name");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");
		
		auto _dragable = wolf::system::w_xml::get_node_attribute(pNode, "Dragable");
		std::transform(_dragable.begin(), _dragable.end(), _dragable.begin(), ::tolower);

		auto _visisble = wolf::system::w_xml::get_node_attribute(pNode, "Visisble");
		std::transform(_visisble.begin(), _visisble.end(), _visisble.begin(), ::tolower);

		auto _minimized = wolf::system::w_xml::get_node_attribute(pNode, "Minimized");
		std::transform(_minimized.begin(), _minimized.end(), _minimized.begin(), ::tolower);
		
		auto _background_color_top_left = wolf::system::w_xml::get_node_attribute(pNode, "BackgroundColorTopLeft");
		auto _background_color_top_right = wolf::system::w_xml::get_node_attribute(pNode, "BackgroundColorTopRight");
		auto _background_color_bottom_left = wolf::system::w_xml::get_node_attribute(pNode, "BackgroundColorBottomLeft");
		auto _background_color_bottom_right = wolf::system::w_xml::get_node_attribute(pNode, "BackgroundColorBottomRight");

		auto _active_background_color_top_left = wolf::system::w_xml::get_node_attribute(pNode, "ActiveBackgroundColorTopLeft");
		auto _active_background_color_top_right = wolf::system::w_xml::get_node_attribute(pNode, "ActiveBackgroundColorTopRight");
		auto _active_background_color_bottom_left = wolf::system::w_xml::get_node_attribute(pNode, "ActiveBackgroundColorBottomLeft");
		auto _active_background_color_bottom_right = wolf::system::w_xml::get_node_attribute(pNode, "ActiveBackgroundColorBottomRight");


		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _enable_caption = wolf::system::w_xml::get_node_attribute(pNode, "EnableCaption");
		std::transform(_enable_caption.begin(), _enable_caption.end(), _enable_caption.begin(), ::tolower);

		auto _caption = wolf::system::w_xml::get_node_attribute_utf8(pNode, "Caption");

		auto _caption_background_color = wolf::system::w_xml::get_node_attribute(pNode, "CaptionBackgroundColor");
		auto _caption_active_background_color = wolf::system::w_xml::get_node_attribute(pNode, "CaptionActiveBackgroundColor");
		auto _caption_height = wolf::system::w_xml::get_node_attribute(pNode, "CaptionHeight");
		auto _caption_margin = wolf::system::w_xml::get_node_attribute(pNode, "CaptionMargin");
		
		auto _caption_font_name = wolf::system::w_xml::get_node_attribute(pNode, "CaptionFontName");
		std::transform(_caption_font_name.begin(), _caption_font_name.end(), _caption_font_name.begin(), ::tolower);

		auto _caption_font_size = wolf::system::w_xml::get_node_attribute(pNode, "CaptionFontSize");
		auto _caption_font_weight = wolf::system::w_xml::get_node_attribute(pNode, "CaptionFontWeight");
		auto _caption_font_style = wolf::system::w_xml::get_node_attribute(pNode, "CaptionFontStyle");
		auto _caption_font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "CaptionFontStretch");
		auto _caption_font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "CaptionFontParagraphAlignment");
		auto _caption_font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "CaptionFontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _caption_margin_values = std::atov2(_caption_margin.c_str());

		_temp_parent_widget_ptr = nullptr;
		//create widget
		auto _hr = add_widget(
			pHwnd,
			_name,
			&_temp_parent_widget_ptr,
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			_dragable == "true" ? true : false,
			_visisble == "true" ? true : false,
			_minimized == "true" ? true : false,
			w_color::from_string(_background_color_top_left.c_str()),
			w_color::from_string(_background_color_top_right.c_str()),
			w_color::from_string(_background_color_bottom_left.c_str()),
			w_color::from_string(_background_color_bottom_right.c_str()),
			w_color::from_string(_active_background_color_top_left.c_str()),
			w_color::from_string(_active_background_color_top_right.c_str()),
			w_color::from_string(_active_background_color_bottom_left.c_str()),
			w_color::from_string(_active_background_color_bottom_right.c_str()),
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_enable_caption == "true" ? true : false,
			_caption,
			w_color::from_string(_caption_background_color.c_str()),
			w_color::from_string(_caption_active_background_color.c_str()),
			static_cast<UINT>(std::atoi(_caption_height.c_str())),
			(int)_caption_margin_values.x,
			(int) _caption_margin_values.y,
			_caption_font_name,
			static_cast<UINT>(std::atoi(_caption_font_size.c_str())),
			static_cast<UINT>(std::atoi(_caption_font_weight.c_str())),
			static_cast<UINT>(std::atoi(_caption_font_style.c_str())),
			static_cast<UINT>(std::atoi(_caption_font_stretch.c_str())),
			static_cast<UINT>(std::atoi(_caption_font_paragraph_alignment.c_str())),
			static_cast<UINT>(std::atoi(_caption_font_text_alignment.c_str())));

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating widget: %s from path: %s", _name, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion

		//read childs
		for (auto _node = pNode->first_node(); _node != nullptr; _node = _node->next_sibling())
		{
			_traversing_gui_node(_node, nullptr, pGuiDesignPath);
		}
	}
	else if (_name == "image")
	{
#pragma region create image from xml

		//check the parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui image without parent. The image must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui image with unsupported parent. The image must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//read xml values
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _path = wolf::system::w_xml::get_node_attribute_utf8(pNode, "Path");
		auto _absolute_path = wolf::system::w_xml::get_node_attribute(pNode, "AbsolutePath");
		std::transform(_absolute_path.begin(), _absolute_path.end(), _absolute_path.begin(), ::tolower);

		auto _use_default_size = wolf::system::w_xml::get_node_attribute(pNode, "UseDefaultSize");
		std::transform(_use_default_size.begin(), _use_default_size.end(), _use_default_size.begin(), ::tolower);

		auto _rotation = wolf::system::w_xml::get_node_attribute(pNode, "Rotation");		
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		
		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _rotation_values = std::atov2(_rotation.c_str());
		
		//create image
		auto _hr = add_image(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			_path,
			_absolute_path == "true" ? true : false,
			_rotation_values.x, _rotation_values.y,
			_use_default_size == "true" ? true : false,
			static_cast<UINT>(_size_values.x),
			static_cast<UINT>(_size_values.y),
			_is_default == "true" ? true : false,
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating label: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
	else if (_name == "label")
	{
#pragma region create label from xml

		//check the parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui label without parent. The label must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui label with unsupported parent. The label must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//read xml values

		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _text = wolf::system::w_xml::get_node_attribute_utf8(pNode, "Text");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MouseOverColor");
		auto _pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "PressedColor");
		auto _focused_color = wolf::system::w_xml::get_node_attribute(pNode, "FocusedColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _font_name = wolf::system::w_xml::get_node_attribute(pNode, "FontName");
		std::transform(_font_name.begin(), _font_name.end(), _font_name.begin(), ::tolower);

		auto _font_size = wolf::system::w_xml::get_node_attribute(pNode, "FontSize");
		auto _font_weight = wolf::system::w_xml::get_node_attribute(pNode, "FontWeight");
		auto _font_style = wolf::system::w_xml::get_node_attribute(pNode, "FontStyle");
		auto _font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "FontStretch");
		auto _font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontParagraphAlignment");
		auto _font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_value = std::atov2(_position.c_str());

		//create label
		auto _hr = add_label(wolf::system::convert::to_hex(_id),
			_text,
			static_cast<UINT>(_position_value.x), static_cast<UINT>(_position_value.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			w_color::from_string(_color.c_str()),
			w_color::from_string(_mouse_over_color.c_str()),
			w_color::from_string(_pressed_color.c_str()),
			w_color::from_string(_focused_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			_is_default == "true" ? true : false,
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_font_name,
			static_cast<UINT>(atoi(_font_size.c_str())),
			static_cast<UINT>(atoi(_font_weight.c_str())),
			static_cast<UINT>(atoi(_font_style.c_str())),
			static_cast<UINT>(atoi(_font_stretch.c_str())),
			static_cast<UINT>(atoi(_font_paragraph_alignment.c_str())),
			static_cast<UINT>(atoi(_font_text_alignment.c_str())));

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating label: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, "w_gui", 3);
		}

#pragma endregion
	}
	else if (_name == "button")
	{
#pragma region create button from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui button without parent. The button must be child of a widget.", "w_gui", 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui button with unsupported parent. The button must be child of a widget.", "w_gui", 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _text = wolf::system::w_xml::get_node_attribute_utf8(pNode, "Text");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");
		auto _icon_path = wolf::system::w_xml::get_node_attribute_utf8(pNode, "IconPath");

		auto _icon_absolute_path = wolf::system::w_xml::get_node_attribute(pNode, "IconAbsolutePath");
		std::transform(_icon_absolute_path.begin(), _icon_absolute_path.end(), _icon_absolute_path.begin(), ::tolower);

		auto _text_offset = wolf::system::w_xml::get_node_attribute(pNode, "TextOffset");
		auto _icon_offset = wolf::system::w_xml::get_node_attribute(pNode, "IconOffset");
		auto _icon_size = wolf::system::w_xml::get_node_attribute(pNode, "IconSize");

		auto _icon_use_default_size = wolf::system::w_xml::get_node_attribute(pNode, "IconUseDefaultSize");
		std::transform(_icon_use_default_size.begin(), _icon_use_default_size.end(), _icon_use_default_size.begin(), ::tolower);

		auto _hot_key = wolf::system::w_xml::get_node_attribute(pNode, "Hotkey");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MouseOverColor");
		auto _pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "PressedColor");
		auto _focused_color = wolf::system::w_xml::get_node_attribute(pNode, "FocusedColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _text_color = wolf::system::w_xml::get_node_attribute(pNode, "TextColor");
		auto _text_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "TextMouseOverColor");
		auto _text_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "TextPressedColor");
		auto _text_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "TextFocusedColor");
		auto _text_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "TextDisabledColor");

		auto _effect_values = wolf::system::w_xml::get_node_attribute(pNode, "EffectValues");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _font_name = wolf::system::w_xml::get_node_attribute(pNode, "FontName");
		std::transform(_font_name.begin(), _font_name.end(), _font_name.begin(), ::tolower);

		auto _font_size = wolf::system::w_xml::get_node_attribute(pNode, "FontSize");
		auto _font_weight = wolf::system::w_xml::get_node_attribute(pNode, "FontWeight");
		auto _font_style = wolf::system::w_xml::get_node_attribute(pNode, "FontStyle");
		auto _font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "FontStretch");
		auto _font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontParagraphAlignment");
		auto _font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());

		glm::vec2 _text_offset_values = std::atov2(_text_offset.c_str());
		glm::vec2 _icon_offset_values = std::atov2(_icon_offset.c_str());
		glm::vec2 _icon_size_values = std::atov2(_icon_size.c_str());

		glm::vec2 _effect_values_values = std::atov2(_effect_values.c_str());

		//create button
		auto _hr = add_button(wolf::system::convert::to_hex(_id),
			_text,
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			_icon_path,
			_icon_absolute_path == "true" ? true : false,
			(int) _text_offset_values.x, (int) _text_offset_values.y,
			(int) _icon_offset_values.x, (int) _icon_offset_values.y,
			_icon_use_default_size == "true" ? true : false,
			_icon_size_values.x, _icon_size_values.y,
			static_cast<UINT>(atoi(_hot_key.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_color.c_str()),
			w_color::from_string(_mouse_over_color.c_str()),
			w_color::from_string(_pressed_color.c_str()),
			w_color::from_string(_focused_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			w_color::from_string(_text_color.c_str()),
			w_color::from_string(_text_mouse_over_color.c_str()),
			w_color::from_string(_text_pressed_color.c_str()),
			w_color::from_string(_text_focused_color.c_str()),
			w_color::from_string(_text_disabled_color.c_str()),
			_effect_values_values.x, _effect_values_values.y,
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_font_name,
			static_cast<UINT>(atoi(_font_size.c_str())),
			static_cast<UINT>(atoi(_font_weight.c_str())),
			static_cast<UINT>(atoi(_font_style.c_str())),
			static_cast<UINT>(atoi(_font_stretch.c_str())),
			static_cast<UINT>(atoi(_font_paragraph_alignment.c_str())),
			static_cast<UINT>(atoi(_font_text_alignment.c_str())));

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating button: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, "w_gui", 3);
		}

#pragma endregion
	}
	else if (_name == "checkbox")
	{
#pragma region create check_box from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui check_box without parent. The check_box must be child of a widget.", "w_gui", 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui check_box with unsupported parent. The check_box must be child of a widget.", "w_gui", 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _text = wolf::system::w_xml::get_node_attribute_utf8(pNode, "Text");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _checked = wolf::system::w_xml::get_node_attribute(pNode, "Checked");
		std::transform(_checked.begin(), _checked.end(), _checked.begin(), ::tolower);

		auto _text_offset = wolf::system::w_xml::get_node_attribute(pNode, "TextOffset");
		auto _hot_key = wolf::system::w_xml::get_node_attribute(pNode, "Hotkey");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MouseOverColor");
		auto _pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "PressedColor");
		auto _focused_color = wolf::system::w_xml::get_node_attribute(pNode, "FocusedColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _text_color = wolf::system::w_xml::get_node_attribute(pNode, "TextColor");
		auto _text_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "TextMouseOverColor");
		auto _text_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "TextPressedColor");
		auto _text_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "TextFocusedColor");
		auto _text_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "TextDisabledColor");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _font_name = wolf::system::w_xml::get_node_attribute(pNode, "FontName");
		std::transform(_font_name.begin(), _font_name.end(), _font_name.begin(), ::tolower);

		auto _font_size = wolf::system::w_xml::get_node_attribute(pNode, "FontSize");
		auto _font_weight = wolf::system::w_xml::get_node_attribute(pNode, "FontWeight");
		auto _font_style = wolf::system::w_xml::get_node_attribute(pNode, "FontStyle");
		auto _font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "FontStretch");
		auto _font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontParagraphAlignment");
		auto _font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _text_offset_values = std::atov2(_text_offset.c_str());

		//create check box
		auto _hr = add_check_box(wolf::system::convert::to_hex(_id),
			_text,
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			_checked == "true" ? true : false,
			(int) _text_offset_values.x, (int) _text_offset_values.y,
			static_cast<UINT>(atoi(_hot_key.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_color.c_str()),
			w_color::from_string(_mouse_over_color.c_str()),
			w_color::from_string(_pressed_color.c_str()),
			w_color::from_string(_focused_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			w_color::from_string(_text_color.c_str()),
			w_color::from_string(_text_mouse_over_color.c_str()),
			w_color::from_string(_text_pressed_color.c_str()),
			w_color::from_string(_text_focused_color.c_str()),
			w_color::from_string(_text_disabled_color.c_str()),
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_font_name,
			static_cast<UINT>(atoi(_font_size.c_str())),
			static_cast<UINT>(atoi(_font_weight.c_str())),
			static_cast<UINT>(atoi(_font_style.c_str())),
			static_cast<UINT>(atoi(_font_stretch.c_str())),
			static_cast<UINT>(atoi(_font_paragraph_alignment.c_str())),
			static_cast<UINT>(atoi(_font_text_alignment.c_str())));

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating check_box: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, "w_gui", 3);
		}

#pragma endregion
	}
	else if (_name == "radiobutton")
	{
#pragma region create radio_button from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui radio_button without parent. The radio_button must be child of a widget.", "w_gui", 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui radio_button with unsupported parent. The radio_button must be child of a widget.", "w_gui", 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _button_group_id = wolf::system::w_xml::get_node_attribute(pNode, "ButtonGroup");
		auto _text = wolf::system::w_xml::get_node_attribute_utf8(pNode, "Text");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _checked = wolf::system::w_xml::get_node_attribute(pNode, "Checked");
		std::transform(_checked.begin(), _checked.end(), _checked.begin(), ::tolower);

		auto _text_offset = wolf::system::w_xml::get_node_attribute(pNode, "TextOffset");
		auto _hot_key = wolf::system::w_xml::get_node_attribute(pNode, "Hotkey");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MouseOverColor");
		auto _pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "PressedColor");
		auto _focused_color = wolf::system::w_xml::get_node_attribute(pNode, "FocusedColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _text_color = wolf::system::w_xml::get_node_attribute(pNode, "TextColor");
		auto _text_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "TextMouseOverColor");
		auto _text_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "TextPressedColor");
		auto _text_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "TextFocusedColor");
		auto _text_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "TextDisabledColor");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _font_name = wolf::system::w_xml::get_node_attribute(pNode, "FontName");
		std::transform(_font_name.begin(), _font_name.end(), _font_name.begin(), ::tolower);

		auto _font_size = wolf::system::w_xml::get_node_attribute(pNode, "FontSize");
		auto _font_weight = wolf::system::w_xml::get_node_attribute(pNode, "FontWeight");
		auto _font_style = wolf::system::w_xml::get_node_attribute(pNode, "FontStyle");
		auto _font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "FontStretch");
		auto _font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontParagraphAlignment");
		auto _font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _text_offset_values = std::atov2(_text_offset.c_str());

		//create radio button
		auto _hr = add_radio_button(wolf::system::convert::to_hex(_id),
			static_cast<UINT>(atoi(_button_group_id.c_str())),
			_text,
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			_checked == "true" ? true : false,
			(int) _text_offset_values.x, (int) _text_offset_values.y,
			static_cast<UINT>(atoi(_hot_key.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_color.c_str()),
			w_color::from_string(_mouse_over_color.c_str()),
			w_color::from_string(_pressed_color.c_str()),
			w_color::from_string(_focused_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			w_color::from_string(_text_color.c_str()),
			w_color::from_string(_text_mouse_over_color.c_str()),
			w_color::from_string(_text_pressed_color.c_str()),
			w_color::from_string(_text_focused_color.c_str()),
			w_color::from_string(_text_disabled_color.c_str()),
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_font_name,
			static_cast<UINT>(atoi(_font_size.c_str())),
			static_cast<UINT>(atoi(_font_weight.c_str())),
			static_cast<UINT>(atoi(_font_style.c_str())),
			static_cast<UINT>(atoi(_font_stretch.c_str())),
			static_cast<UINT>(atoi(_font_paragraph_alignment.c_str())),
			static_cast<UINT>(atoi(_font_text_alignment.c_str())));

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating radio_button: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, "w_gui", 3);
		}

#pragma endregion
	}
	else if (_name == "combobox")
	{
#pragma region create combo_box from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui combo_box without parent. The combo_box must be child of a widget.", "w_gui", 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui combo_box with unsupported parent. The combo_box must be child of a widget.", "w_gui", 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");
		
		auto _text_offset = wolf::system::w_xml::get_node_attribute(pNode, "TextOffset");
		auto _item_height = wolf::system::w_xml::get_node_attribute(pNode, "ItemHeight");
		auto _item_margin = wolf::system::w_xml::get_node_attribute(pNode, "ItemMargin");

		auto _hot_key = wolf::system::w_xml::get_node_attribute(pNode, "Hotkey");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MouseOverColor");
		auto _pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "PressedColor");
		auto _focused_color = wolf::system::w_xml::get_node_attribute(pNode, "FocusedColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _button_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonColor");
		auto _button_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonMouseOverColor");
		auto _button_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonPressedColor");
		auto _button_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonFocusedColor");
		auto _button_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonDisabledColor");

		auto _drop_down_context_color = wolf::system::w_xml::get_node_attribute(pNode, "DropDownContextColor");
		auto _selection_color = wolf::system::w_xml::get_node_attribute(pNode, "SelectionColor");
		auto _selection_font_color = wolf::system::w_xml::get_node_attribute(pNode, "SelectionFontColor");
		auto _text_color = wolf::system::w_xml::get_node_attribute(pNode, "TextColor");
		auto _text_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "TextDisabledColor");
		
		auto _drop_down_context_height = wolf::system::w_xml::get_node_attribute(pNode, "DropDownContextHeight");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _font_name = wolf::system::w_xml::get_node_attribute(pNode, "FontName");
		std::transform(_font_name.begin(), _font_name.end(), _font_name.begin(), ::tolower);

		auto _font_size = wolf::system::w_xml::get_node_attribute(pNode, "FontSize");
		auto _font_weight = wolf::system::w_xml::get_node_attribute(pNode, "FontWeight");
		auto _font_style = wolf::system::w_xml::get_node_attribute(pNode, "FontStyle");
		auto _font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "FontStretch");
		auto _font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontParagraphAlignment");
		auto _font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _text_offset_values = std::atov2(_text_offset.c_str());
		glm::vec2 _item_margin_values = std::atov2(_item_margin.c_str());

		POINT _item_margin_point;
		_item_margin_point.x = static_cast<LONG>(_item_margin_values.x);
		_item_margin_point.y = static_cast<LONG>(_item_margin_values.y);

		//read combo box items
		std::vector<std::wstring> _combo_box_items;
		for (auto _node = pNode->first_node(); _node != nullptr; _node = _node->next_sibling())
		{
			if (!_node) continue;

			//get name
			auto _item_name = std::string(_node->name());
			std::transform(_item_name.begin(), _item_name.end(), _item_name.begin(), ::tolower);

			if (_item_name == "comboboxitem")
			{
				auto _item = wolf::system::convert::from_utf8(_node->value());
				_combo_box_items.push_back(_item);
			}
		}

		//create combo box
		auto _hr = add_combo_box(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			(int) _text_offset_values.x, (int) _text_offset_values.y,
			static_cast<UINT>(atoi(_item_height.c_str())),
			_item_margin_point,
			static_cast<UINT>(atoi(_hot_key.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_color.c_str()),
			w_color::from_string(_mouse_over_color.c_str()),
			w_color::from_string(_pressed_color.c_str()),
			w_color::from_string(_focused_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			w_color::from_string(_button_color.c_str()),
			w_color::from_string(_button_mouse_over_color.c_str()),
			w_color::from_string(_button_pressed_color.c_str()),
			w_color::from_string(_button_focused_color.c_str()),
			w_color::from_string(_button_disabled_color.c_str()),
			w_color::from_string(_drop_down_context_color.c_str()),
			w_color::from_string(_selection_color.c_str()),
			w_color::from_string(_selection_font_color.c_str()),
			w_color::from_string(_text_color.c_str()),
			w_color::from_string(_text_disabled_color.c_str()),
			static_cast<UINT>(atoi(_drop_down_context_height.c_str())),
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_font_name,
			static_cast<UINT>(atoi(_font_size.c_str())),
			static_cast<UINT>(atoi(_font_weight.c_str())),
			static_cast<UINT>(atoi(_font_style.c_str())),
			static_cast<UINT>(atoi(_font_stretch.c_str())),
			static_cast<UINT>(atoi(_font_paragraph_alignment.c_str())),
			static_cast<UINT>(atoi(_font_text_alignment.c_str())),
			_combo_box_items);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating combo_box: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, "w_gui", 3);
		}

#pragma endregion
	}
	else if (_name == "slider")
	{
#pragma region create slider from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui slider without parent. The slider must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui slider with unsupported parent. The slider must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _range = wolf::system::w_xml::get_node_attribute(pNode, "Range");
		auto _value = wolf::system::w_xml::get_node_attribute(pNode, "Value");

		auto _hot_key = wolf::system::w_xml::get_node_attribute(pNode, "Hotkey");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _main_color = wolf::system::w_xml::get_node_attribute(pNode, "MainColor");
		auto _main_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MainMouseOverColor");
		auto _main_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "MainPressedColor");
		auto _main_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "MainFocusedColor");
		auto _main_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "MainDisabledColor");

		auto _button_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonColor");
		auto _button_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonMouseOverColor");
		auto _button_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonPressedColor");
		auto _button_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonFocusedColor");
		auto _button_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonDisabledColor");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _font_name = wolf::system::w_xml::get_node_attribute(pNode, "FontName");
		std::transform(_font_name.begin(), _font_name.end(), _font_name.begin(), ::tolower);

		auto _font_size = wolf::system::w_xml::get_node_attribute(pNode, "FontSize");
		auto _font_weight = wolf::system::w_xml::get_node_attribute(pNode, "FontWeight");
		auto _font_style = wolf::system::w_xml::get_node_attribute(pNode, "FontStyle");
		auto _font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "FontStretch");
		auto _font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontParagraphAlignment");
		auto _font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _range_values = std::atov2(_range.c_str());

		//create slider
		auto _hr = add_slider(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			(int) _range_values.x, (int) _range_values.y,
			static_cast<int>(atoi(_value.c_str())),
			static_cast<UINT>(atoi(_hot_key.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_main_color.c_str()),
			w_color::from_string(_main_mouse_over_color.c_str()),
			w_color::from_string(_main_pressed_color.c_str()),
			w_color::from_string(_main_focused_color.c_str()),
			w_color::from_string(_main_disabled_color.c_str()),
			w_color::from_string(_button_color.c_str()),
			w_color::from_string(_button_mouse_over_color.c_str()),
			w_color::from_string(_button_pressed_color.c_str()),
			w_color::from_string(_button_focused_color.c_str()),
			w_color::from_string(_button_disabled_color.c_str()),
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_font_name,
			static_cast<UINT>(atoi(_font_size.c_str())),
			static_cast<UINT>(atoi(_font_weight.c_str())),
			static_cast<UINT>(atoi(_font_style.c_str())),
			static_cast<UINT>(atoi(_font_stretch.c_str())),
			static_cast<UINT>(atoi(_font_paragraph_alignment.c_str())),
			static_cast<UINT>(atoi(_font_text_alignment.c_str())));

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating slider: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
	else if (_name == "listbox")
	{
#pragma region create list_box from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui listbox without parent. The listbox must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui listbox with unsupported parent. The listbox must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _multi_selection = wolf::system::w_xml::get_node_attribute(pNode, "MultiSelection");
		auto _border = wolf::system::w_xml::get_node_attribute(pNode, "Border");
		auto _text_offset = wolf::system::w_xml::get_node_attribute(pNode, "TextOffset");
		auto _icon_margin = wolf::system::w_xml::get_node_attribute(pNode, "IconMargin");
		auto _icon_size = wolf::system::w_xml::get_node_attribute(pNode, "IconSize");

		auto _icon_use_default_size = wolf::system::w_xml::get_node_attribute(pNode, "IconUseDefaultSize");
		std::transform(_icon_use_default_size.begin(), _icon_use_default_size.end(), _icon_use_default_size.begin(), ::tolower);

		auto _selected_margin = wolf::system::w_xml::get_node_attribute(pNode, "SelectedRectangleMargin");
		auto _item_height = wolf::system::w_xml::get_node_attribute(pNode, "ItemHeight");
		auto _icon_height_offset = wolf::system::w_xml::get_node_attribute(pNode, "IconHeightOffset");
				
		auto _text_color = wolf::system::w_xml::get_node_attribute(pNode, "TextColor");
		auto _selected_text_color = wolf::system::w_xml::get_node_attribute(pNode, "SelectedTextColor");
		auto _disabled_text_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledTextColor");

		auto _color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _selected_color = wolf::system::w_xml::get_node_attribute(pNode, "SelectedColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _scroll_color = wolf::system::w_xml::get_node_attribute(pNode, "ScrollColor");
		auto _scroll_background_color = wolf::system::w_xml::get_node_attribute(pNode, "ScrollBackgroundColor");
		auto _scroll_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "ScrollDisabledColor");
		
		auto _hot_key = wolf::system::w_xml::get_node_attribute(pNode, "Hotkey");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		auto _font_name = wolf::system::w_xml::get_node_attribute(pNode, "FontName");
		std::transform(_font_name.begin(), _font_name.end(), _font_name.begin(), ::tolower);

		auto _font_size = wolf::system::w_xml::get_node_attribute(pNode, "FontSize");
		auto _font_weight = wolf::system::w_xml::get_node_attribute(pNode, "FontWeight");
		auto _font_style = wolf::system::w_xml::get_node_attribute(pNode, "FontStyle");
		auto _font_stretch = wolf::system::w_xml::get_node_attribute(pNode, "FontStretch");
		auto _font_paragraph_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontParagraphAlignment");
		auto _font_text_alignment = wolf::system::w_xml::get_node_attribute(pNode, "FontTextAlignment");

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _border_values = std::atov2(_border.c_str());
		glm::vec2 _text_offset_values = std::atov2(_text_offset.c_str());
		glm::vec2 _icon_margin_values = std::atov2(_icon_margin.c_str());
		glm::vec2 _icon_size_values = std::atov2(_icon_size.c_str());
		glm::vec2 _selected_margin_values = std::atov2(_selected_margin.c_str());

		//read tab items
		std::vector<std::tuple<std::wstring, std::wstring, bool>> _items;
		for (auto _node = pNode->first_node(); _node != nullptr; _node = _node->next_sibling())
		{
			if (!_node) continue;

			//get name
			auto _item_name = std::string(_node->name());
			std::transform(_item_name.begin(), _item_name.end(), _item_name.begin(), ::tolower);

			if (_item_name == "listboxitem")
			{
				auto _text = wolf::system::w_xml::get_node_attribute_utf8(_node, "Text");
				auto _icon_path = wolf::system::w_xml::get_node_attribute_utf8(_node, "IconPath");
				
				auto _icon_absolute_path = wolf::system::w_xml::get_node_attribute(_node, "IconAbsolutePath");
				std::transform(_icon_absolute_path.begin(), _icon_absolute_path.end(), _icon_absolute_path.begin(), ::tolower);
			
				auto _item = std::make_tuple(_text,
					_icon_path,
					_icon_absolute_path == "true" ? true : false);
				_items.push_back(_item);
			}
		}

		//create list box
		auto _hr = add_list_box(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			_multi_selection == "true" ? true : false,
			static_cast<int>(_border_values.x), static_cast<int>(_border_values.y),
			static_cast<int>(_text_offset_values.x), static_cast<int>(_text_offset_values.y),
			static_cast<int>(_icon_margin_values.x), static_cast<int>(_icon_margin_values.y),
			_icon_size_values.x, _icon_size_values.y,
			_icon_use_default_size == "true" ? true : false,
			static_cast<int>(_selected_margin_values.x), static_cast<int>(_selected_margin_values.y),
			atoi(_item_height.c_str()),
			atoi(_icon_height_offset.c_str()),
			static_cast<UINT>(atoi(_hot_key.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_text_color.c_str()),
			w_color::from_string(_selected_text_color.c_str()),
			w_color::from_string(_disabled_text_color.c_str()),
			w_color::from_string(_color.c_str()),
			w_color::from_string(_selected_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			w_color::from_string(_scroll_color.c_str()),
			w_color::from_string(_scroll_background_color.c_str()),
			w_color::from_string(_scroll_disabled_color.c_str()),
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_font_name,
			static_cast<UINT>(atoi(_font_size.c_str())),
			static_cast<UINT>(atoi(_font_weight.c_str())),
			static_cast<UINT>(atoi(_font_style.c_str())),
			static_cast<UINT>(atoi(_font_stretch.c_str())),
			static_cast<UINT>(atoi(_font_paragraph_alignment.c_str())),
			static_cast<UINT>(atoi(_font_text_alignment.c_str())),
			_items);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating listbox: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
	else if (_name == "listwidget")
	{
#pragma region create list_widget from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui list_widget without parent. The list_widget must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui list_widget with unsupported parent. The list_widget must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _multi_selection = wolf::system::w_xml::get_node_attribute(pNode, "MultiSelection");
		auto _border = wolf::system::w_xml::get_node_attribute(pNode, "Border");
		auto _selected_margin = wolf::system::w_xml::get_node_attribute(pNode, "SelectedRectangleMargin");
		auto _item_height = wolf::system::w_xml::get_node_attribute(pNode, "ItemHeight");

		auto _color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _selected_color = wolf::system::w_xml::get_node_attribute(pNode, "SelectedColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _scroll_color = wolf::system::w_xml::get_node_attribute(pNode, "ScrollColor");
		auto _scroll_background_color = wolf::system::w_xml::get_node_attribute(pNode, "ScrollBackgroundColor");
		auto _scroll_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "ScrollDisabledColor");

		auto _hot_key = wolf::system::w_xml::get_node_attribute(pNode, "Hotkey");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _border_values = std::atov2(_border.c_str());
		glm::vec2 _selected_margin_values = std::atov2(_selected_margin.c_str());

		//create list box
		auto _hr = add_list_widget(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			_multi_selection == "true" ? true : false,
			static_cast<int>(_border_values.x), static_cast<int>(_border_values.y),
			static_cast<int>(_selected_margin_values.x), static_cast<int>(_selected_margin_values.y),
			atoi(_item_height.c_str()),
			static_cast<UINT>(atoi(_hot_key.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_color.c_str()),
			w_color::from_string(_selected_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			w_color::from_string(_scroll_color.c_str()),
			w_color::from_string(_scroll_background_color.c_str()),
			w_color::from_string(_scroll_disabled_color.c_str()),
			static_cast<float>(atof(_z_order.c_str())),
			_enabled == "true" ? true : false);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating list_widget: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
	else if (_name == "tab")
	{
#pragma region create tab from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui tab without parent. The tab must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui tab with unsupported parent. The tab must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");

		auto _tab_button_size = wolf::system::w_xml::get_node_attribute(pNode, "TabButtonSize");
		auto _arrow_button_text_offset = wolf::system::w_xml::get_node_attribute(pNode, "ArrowButtonTextOffset");

		auto _button_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonColor");
		auto _button_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonMouseOverColor");
		auto _button_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonPressedColor");
		auto _button_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonFocusedColor");
		auto _button_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "ButtonDisabledColor");

		auto _text_color = wolf::system::w_xml::get_node_attribute(pNode, "TextColor");
		auto _text_mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "TextMouseOverColor");
		auto _text_pressed_color = wolf::system::w_xml::get_node_attribute(pNode, "TextPressedColor");
		auto _text_focused_color = wolf::system::w_xml::get_node_attribute(pNode, "TextFocusedColor");
		auto _text_disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "TextDisabledColor");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);
		
		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _position_values = std::atov2(_position.c_str());

		glm::vec2 _tab_button_size_values = std::atov2(_tab_button_size.c_str());
		glm::vec2 _arrow_button_text_offset_values = std::atov2(_arrow_button_text_offset.c_str());

		//read tab items
		std::vector<std::tuple<std::wstring, std::wstring, bool, glm::vec2, glm::vec2, glm::vec2, int, bool>> _items;
		for (auto _node = pNode->first_node(); _node != nullptr; _node = _node->next_sibling())
		{
			if (!_node) continue;

			//get name
			auto _item_name = std::string(_node->name());
			std::transform(_item_name.begin(), _item_name.end(), _item_name.begin(), ::tolower);

			if (_item_name == "tabitem")
			{
				auto _text = wolf::system::w_xml::get_node_attribute_utf8(_node, "Text");
				auto _icon_path = wolf::system::w_xml::get_node_attribute_utf8(_node, "IconPath");
				
				auto _icon_absolute_path = wolf::system::w_xml::get_node_attribute(_node, "IconAbsolutePath");
				std::transform(_icon_absolute_path.begin(), _icon_absolute_path.end(), _icon_absolute_path.begin(), ::tolower);

				auto _text_offset = wolf::system::w_xml::get_node_attribute(_node, "TextOffset");
				auto _icon_offset = wolf::system::w_xml::get_node_attribute(_node, "IconOffset");
				auto _icon_scale = wolf::system::w_xml::get_node_attribute(_node, "IconScale");
				auto _hot_key = wolf::system::w_xml::get_node_attribute(_node, "HotKey");
				
				auto _is_default = wolf::system::w_xml::get_node_attribute(_node, "IsDefault");
				std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

				auto _item = std::make_tuple(_text, 
					_icon_path, 
					_icon_absolute_path == "true" ? true : false,
					std::atov2(_text_offset.c_str()), 
					std::atov2(_icon_offset.c_str()),
					std::atov2(_icon_scale.c_str()),
					std::atoi(_hot_key.c_str()), 
					_is_default == "true" ? true : false);
				_items.push_back(_item);
			}
		}

		//create tab
		auto _hr = add_tab(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<UINT>(_size_values.x), static_cast<UINT>(_size_values.y),
			static_cast<UINT>(_tab_button_size_values.x), static_cast<UINT>(_tab_button_size_values.y),
			static_cast<int>(_arrow_button_text_offset_values.x), static_cast<int>(_arrow_button_text_offset_values.y),
			w_color::from_string(_button_color.c_str()),
			w_color::from_string(_button_mouse_over_color.c_str()),
			w_color::from_string(_button_pressed_color.c_str()),
			w_color::from_string(_button_focused_color.c_str()),
			w_color::from_string(_button_disabled_color.c_str()),
			w_color::from_string(_text_color.c_str()),
			w_color::from_string(_text_mouse_over_color.c_str()),
			w_color::from_string(_text_pressed_color.c_str()),
			w_color::from_string(_text_focused_color.c_str()),
			w_color::from_string(_text_disabled_color.c_str()),
			static_cast<float>(std::atof(_z_order.c_str())),
			_enabled == "true" ? true : false,
			_items);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating combo_box: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
	else if (_name == "line")
	{
#pragma region create line shape from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui line shape without parent. The line shape must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui line shape with unsupported parent. The line shape must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _start_point = wolf::system::w_xml::get_node_attribute(pNode, "StartPoint");
		auto _stop_point = wolf::system::w_xml::get_node_attribute(pNode, "StopPoint");

		auto _stroke_width = wolf::system::w_xml::get_node_attribute(pNode, "StrokeWidth");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);
		
		auto _fill_color = wolf::system::w_xml::get_node_attribute(pNode, "Color");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		glm::vec2 _start_point_values = std::atov2(_start_point.c_str());
		glm::vec2 _stop_point_values = std::atov2(_stop_point.c_str());

		//create line
		auto _hr = add_line_shape(wolf::system::convert::to_hex(_id),
			static_cast<int>(_start_point_values.x), static_cast<int>(_start_point_values.y),
			static_cast<int>(_stop_point_values.x), static_cast<int>(_stop_point_values.y),
			static_cast<UINT>(atoi(_stroke_width.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_fill_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			static_cast<float>(std::atof(_z_order.c_str())),
			_enabled == "true" ? true : false);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating line shape: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
	else if (_name == "rectangle")
	{
#pragma region create rectangle shape from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui rectangle shape without parent. The rectangle shape must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui rectangle shape with unsupported parent. The rectangle shape must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _size = wolf::system::w_xml::get_node_attribute(pNode, "Size");
		auto _radius = wolf::system::w_xml::get_node_attribute(pNode, "Radius");
		auto _stroke_width = wolf::system::w_xml::get_node_attribute(pNode, "StrokeWidth");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _fill_color = wolf::system::w_xml::get_node_attribute(pNode, "FillColor");
		auto _border_color = wolf::system::w_xml::get_node_attribute(pNode, "BorderColor");
		auto _mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MouseOverColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _size_values = std::atov2(_size.c_str());
		glm::vec2 _radius_values = std::atov2(_radius.c_str());

		//create rectangle
		auto _hr = add_rounded_rectangle_shape(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<int>(_size_values.x), static_cast<int>(_size_values.y),
			static_cast<int>(_radius_values.x), static_cast<int>(_radius_values.y),
			static_cast<UINT>(atoi(_stroke_width.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_fill_color.c_str()),
			w_color::from_string(_border_color.c_str()),
			w_color::from_string(_mouse_over_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			static_cast<float>(std::atof(_z_order.c_str())),
			_enabled == "true" ? true : false);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating rectangle shape: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
	else if (_name == "ellipse")
	{
#pragma region create ellipse shape from xml

		//check parent
		auto _parent_node = pNode->parent();
		if (_parent_node == nullptr || _temp_parent_widget_ptr == nullptr)
		{
			V(S_FALSE, L"loading a gui ellipse shape without parent. The ellipse shape must be child of a widget.", _trace_class_name, 2);
			return;
		}
		else
		{
			auto _parent_node_name = std::string(_parent_node->name());
			std::transform(_parent_node_name.begin(), _parent_node_name.end(), _parent_node_name.begin(), ::tolower);
			if (_parent_node_name != "widget")
			{
				V(S_FALSE, L"loading a gui ellipse shape with unsupported parent. The ellipse shape must be child of a widget.", _trace_class_name, 2);
				return;
			}
		}

		//load xml value
		auto _id = wolf::system::w_xml::get_node_attribute(pNode, "ID");
		auto _position = wolf::system::w_xml::get_node_attribute(pNode, "Position");
		auto _radius = wolf::system::w_xml::get_node_attribute(pNode, "Radius");
		auto _stroke_width = wolf::system::w_xml::get_node_attribute(pNode, "StrokeWidth");

		auto _is_default = wolf::system::w_xml::get_node_attribute(pNode, "IsDefault");
		std::transform(_is_default.begin(), _is_default.end(), _is_default.begin(), ::tolower);

		auto _fill_color = wolf::system::w_xml::get_node_attribute(pNode, "FillColor");
		auto _border_color = wolf::system::w_xml::get_node_attribute(pNode, "BorderColor");
		auto _mouse_over_color = wolf::system::w_xml::get_node_attribute(pNode, "MouseOverColor");
		auto _disabled_color = wolf::system::w_xml::get_node_attribute(pNode, "DisabledColor");

		auto _z_order = wolf::system::w_xml::get_node_attribute(pNode, "ZOrder");

		auto _enabled = wolf::system::w_xml::get_node_attribute(pNode, "Enabled");
		std::transform(_enabled.begin(), _enabled.end(), _enabled.begin(), ::tolower);

		glm::vec2 _position_values = std::atov2(_position.c_str());
		glm::vec2 _radius_values = std::atov2(_radius.c_str());

		//create ellipse
		auto _hr = add_ellipse_shape(wolf::system::convert::to_hex(_id),
			static_cast<int>(_position_values.x), static_cast<int>(_position_values.y),
			static_cast<int>(_radius_values.x), static_cast<int>(_radius_values.y),
			static_cast<UINT>(atoi(_stroke_width.c_str())),
			_is_default == "true" ? true : false,
			w_color::from_string(_fill_color.c_str()),
			w_color::from_string(_border_color.c_str()),
			w_color::from_string(_mouse_over_color.c_str()),
			w_color::from_string(_disabled_color.c_str()),
			static_cast<float>(std::atof(_z_order.c_str())),
			_enabled == "true" ? true : false);

		if (_hr == S_FALSE)
		{
			wchar_t _msg[2048];
			wsprintf(_msg, L"creating ellipse shape: %s from path: %s", _id, pGuiDesignPath);
			V(S_FALSE, _msg, _trace_class_name, 3);
		}

#pragma endregion
	}
}

HRESULT w_gui::add_widget(_In_ HWND pHwnd,
	_In_ const std::string pWidgetName,
	_Inout_ wolf::gui::w_widget** pWidget,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ bool pDraggable,
	_In_ bool pVisisble,
	_In_ bool pMinimized,
	_In_ w_color pBackgroundColorTopLeft,
	_In_ w_color pBackgroundColorTopRight,
	_In_ w_color pBackgroundColorBottomLeft,
	_In_ w_color pBackgroundColorBottomRight,
	_In_ w_color pActiveBackgroundColorTopLeft,
	_In_ w_color pActiveBackgroundColorTopRight,
	_In_ w_color pActiveBackgroundColorBottomLeft,
	_In_ w_color pActiveBackgroundColorBottomRight,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ bool pEnableCaption,
	_In_ const std::wstring pCaption,
	_In_ w_color pCaptionBackgroundColor,
	_In_ w_color pCaptionActiveBackgroundColor,
	_In_ UINT pCaptionHeight,
	_In_ int pCaptionMarginLeft,
	_In_ int pCaptionMarginTop,
	_In_ const std::string pCaptionFontName,
	_In_ UINT pCaptionFontSize,
	_In_ UINT pCaptionFontWeight,
	_In_ UINT pCaptionFontStyle,
	_In_ UINT pCaptionFontStretch,
	_In_ UINT pCaptionFontParagraphAlignment,
	_In_ UINT pCaptionFontTextAlignment)
{
	auto _find = _widgets.find(pWidgetName);
	if (_find != _widgets.end())
	{
		auto _msg = std::string("widget with name: ") + pWidgetName + std::string(" already exists");
		V(S_FALSE, _msg, _trace_class_name, 3);
		return S_FALSE;
	}

	auto _widget = new wolf::gui::w_widget(pHwnd, _parent_width, _parent_height);
	_widget->initialize(_gDevice, _sprite_batch, _widget_resource_manager);

	_widget->set_width(pWidth);
	_widget->set_height(pHeight);
	_widget->set_draggable(pDraggable);
	_widget->set_visibility(pVisisble);
	_widget->set_minimized(pMinimized);
	_widget->set_position(pX, pY);

	_widget->set_background_colors(pBackgroundColorTopLeft, 
		pBackgroundColorTopRight, 
		pBackgroundColorBottomLeft, 
		pBackgroundColorBottomRight);

	_widget->set_active_background_colors(pActiveBackgroundColorTopLeft, 
		pActiveBackgroundColorTopRight, 
		pActiveBackgroundColorBottomLeft, 
		pActiveBackgroundColorBottomRight);

	_widget->set_z_order(pZOrder);
	_widget->set_enabled(pEnabled);
	_widget->set_enable_caption(pEnableCaption);
	_widget->set_caption(pCaption.c_str());
	_widget->set_caption_background_color(pCaptionBackgroundColor);
	_widget->set_caption_active_background_color(pCaptionActiveBackgroundColor);
	_widget->set_caption_height(pCaptionHeight);
	_widget->set_caption_margin_left(pCaptionMarginLeft);
	_widget->set_caption_margin_top(pCaptionMarginTop);

	if (pWidget)
	{
		*pWidget = _widget;
	}

	_widgets[pWidgetName] = _widget;

	return S_OK;
}

void w_gui::remove_widget(_In_ const std::string pWidgetName)
{
	auto _find = _widgets.find(pWidgetName);
	if (_find != _widgets.end())
	{
		auto _widget = _widgets[pWidgetName];
		if (_widget)
		{
			_widget->release();
		}

		_widgets.erase(pWidgetName);
	}
}

HRESULT w_gui::add_image(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_z_ std::wstring pPath,
	_In_ bool pAbsolutePath,
	_In_ float pRotationX, _In_ float pRotationY,
	_In_ bool pUseDefaultSize,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ bool pIsDefault,
	_In_ float pZOrder,
	_In_ bool pEnabled)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_image* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_image(pID,
			pX, pY,
			pPath, pAbsolutePath,
			pRotationX, pRotationY,
			pUseDefaultSize,
			pWidth, pHeight,
			pIsDefault,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating image with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_label(_In_ int pID,
	_In_z_ std::wstring pText,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ w_color pColor,
	_In_ w_color pMouseOverColor,
	_In_ w_color pPressedColor,
	_In_ w_color pFocusedColor,
	_In_ w_color pDisabledColor,
	_In_ bool pIsDefault,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ const std::string pFontName,
	_In_ UINT pFontSize,
	_In_ UINT pFontWeight,
	_In_ UINT pFontStyle,
	_In_ UINT pFontStretch,
	_In_ UINT pFontParagraphAlignment,
	_In_ UINT pFontTextAlignment)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_label* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_label(pID,
			pText,
			pX, pY,
			pWidth, pHeight,
			pIsDefault,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating label with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
			_control->set_label_color(pColor);
			_control->set_label_mouse_over_color(pMouseOverColor);
			_control->set_label_pressed_color(pPressedColor);
			_control->set_label_focused_color(pFocusedColor);
			_control->set_label_disabled_color(pDisabledColor);			
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_button(_In_ int pID,
	_In_z_ std::wstring pText,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_z_ std::wstring pIconPath,
	_In_ bool pAbsolutePath,
	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
	_In_ int pIconOffsetX, _In_ int pIconOffsetY,
	_In_ bool pIconUseDefaultSize,
	_In_ UINT pIconWidth, _In_ UINT pIconHeight,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ w_color pColor,
	_In_ w_color pMouseOverColor,
	_In_ w_color pPressedColor,
	_In_ w_color pFocusedColor,
	_In_ w_color pDisabledColor,
	_In_ w_color pTextColor,
	_In_ w_color pTextMouseOverColor,
	_In_ w_color pTextPressedColor,
	_In_ w_color pTextFocusedColor,
	_In_ w_color pTextDisabledColor,
	_In_ float pEffectMinValue, _In_ float pEffectMaxValue,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ const std::string pFontName,
	_In_ UINT pFontSize,
	_In_ UINT pFontWeight,
	_In_ UINT pFontStyle,
	_In_ UINT pFontStretch,
	_In_ UINT pFontParagraphAlignment,
	_In_ UINT pFontTextAlignment)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_button* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_button(pID,
			pText,
			pX, pY,
			pWidth, pHeight,
			pIconPath,
			pAbsolutePath,
			pTextOffsetX, pTextOffsetY,
			pIconOffsetX, pIconOffsetY,
			pIconUseDefaultSize,
			pIconWidth, pIconHeight,
			pHotkey,
			pIsDefault,
			pColor, pMouseOverColor, pPressedColor, pFocusedColor, pDisabledColor,
			pTextColor, pTextMouseOverColor, pTextPressedColor, pTextFocusedColor, pTextDisabledColor,
			pEffectMinValue, pEffectMaxValue,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating button with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
			_control->set_label_color(pTextColor);
			_control->set_label_mouse_over_color(pTextMouseOverColor);
			_control->set_label_pressed_color(pTextPressedColor);
			_control->set_label_focused_color(pTextFocusedColor);
			_control->set_label_disabled_color(pTextDisabledColor);
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_check_box(_In_ int pID,
	_In_z_ std::wstring pText,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ bool pChecked,
	_In_ int pTextOffsetX,
	_In_ int pTextOffsetY,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ w_color pColor,
	_In_ w_color pMouseOverColor,
	_In_ w_color pPressedColor,
	_In_ w_color pFocusedColor,
	_In_ w_color pDisabledColor,
	_In_ w_color pTextColor,
	_In_ w_color pTextMouseOverColor,
	_In_ w_color pTextPressedColor,
	_In_ w_color pTextFocusedColor,
	_In_ w_color pTextDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ const std::string pFontName,
	_In_ UINT pFontSize,
	_In_ UINT pFontWeight,
	_In_ UINT pFontStyle,
	_In_ UINT pFontStretch,
	_In_ UINT pFontParagraphAlignment,
	_In_ UINT pFontTextAlignment)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_check_box* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_check_box(pID,
			pText,
			pX, pY,
			pWidth, pHeight,
			pChecked,
			pTextOffsetX, pTextOffsetY,
			pHotkey,
			pIsDefault,
			pColor,
			pMouseOverColor,
			pPressedColor,
			pFocusedColor,
			pDisabledColor,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating check_box with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
			_control->set_label_color(pTextColor);
			_control->set_label_mouse_over_color(pTextMouseOverColor);
			_control->set_label_pressed_color(pTextPressedColor);
			_control->set_label_focused_color(pTextFocusedColor);
			_control->set_label_disabled_color(pTextDisabledColor);
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_radio_button(int pID,
	UINT pButtonGroup,
	_In_z_ std::wstring pText,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ bool pChecked,
	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ w_color pColor,
	_In_ w_color pMouseOverColor,
	_In_ w_color pPressedColor,
	_In_ w_color pFocusedColor,
	_In_ w_color pDisabledColor,
	_In_ w_color pTextColor,
	_In_ w_color pTextMouseOverColor,
	_In_ w_color pTextPressedColor,
	_In_ w_color pTextFocusedColor,
	_In_ w_color pTextDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ const std::string pFontName,
	_In_ UINT pFontSize,
	_In_ UINT pFontWeight,
	_In_ UINT pFontStyle,
	_In_ UINT pFontStretch,
	_In_ UINT pFontParagraphAlignment,
	_In_ UINT pFontTextAlignment)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_radio_button* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_radio_button(pID,
			pButtonGroup,
			pText,
			pX, pY,
			pWidth, pHeight,
			pChecked,
			pTextOffsetX, pTextOffsetY,
			pHotkey,
			pIsDefault,
			pColor,
			pMouseOverColor,
			pPressedColor,
			pFocusedColor,
			pDisabledColor,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating radio_button with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
			_control->set_label_color(pTextColor);
			_control->set_label_mouse_over_color(pTextMouseOverColor);
			_control->set_label_pressed_color(pTextPressedColor);
			_control->set_label_focused_color(pTextFocusedColor);
			_control->set_label_disabled_color(pTextDisabledColor);
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_combo_box(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
	_In_ UINT pItemHeight,
	_In_ POINT pItemMargin,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ w_color pColor,
	_In_ w_color pMouseOverColor,
	_In_ w_color pPressedColor,
	_In_ w_color pFocusedColor,
	_In_ w_color pDisabledColor,
	_In_ w_color pButtonColor,
	_In_ w_color pButtonMouseOverColor,
	_In_ w_color pButtonPressedColor,
	_In_ w_color pButtonFocusedColor,
	_In_ w_color pButtonDisabledColor,
	_In_ w_color pDropDownColor,
	_In_ w_color pSelectionColor,
	_In_ w_color pSelectionFontColor,
	_In_ w_color pTextColor,
	_In_ w_color pTextDisabledColor,
	_In_ UINT pDropDownContextHeight,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ const std::string pFontName,
	_In_ UINT pFontSize,
	_In_ UINT pFontWeight,
	_In_ UINT pFontStyle,
	_In_ UINT pFontStretch,
	_In_ UINT pFontParagraphAlignment,
	_In_ UINT pFontTextAlignment,
	_In_ std::vector<std::wstring>& pItems)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_combo_box* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_combo_box(pID,
			pX, pY,
			pWidth, pHeight,
			pTextOffsetX, pTextOffsetY,
			pHotkey,
			pIsDefault,
			pColor, pMouseOverColor, pPressedColor, pFocusedColor, pDisabledColor,
			pButtonColor, pButtonMouseOverColor, pButtonPressedColor, pButtonFocusedColor, pButtonDisabledColor,
			pDropDownColor, pSelectionColor, pSelectionFontColor,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating combo_box with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_item_height(pItemHeight);
			_control->set_item_margin(static_cast<int>(pItemMargin.x), static_cast<int>(pItemMargin.y));
			_control->set_drop_down_context_height(pDropDownContextHeight);
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
			_control->set_label_color(pTextColor);
			_control->set_label_disabled_color(pTextDisabledColor);

			for (size_t i = 0; i < pItems.size(); ++i)
			{
				_control->add_item(pItems.at(i).c_str(), nullptr);
			}
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_slider(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ int pMin, _In_ int pMax,
	_In_ int pValue,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ w_color pMainColor,
	_In_ w_color pMainMouseOverColor,
	_In_ w_color pMainPressedColor,
	_In_ w_color pMainFocusedColor,
	_In_ w_color pMainDisabledColor,
	_In_ w_color pButtonColor,
	_In_ w_color pButtonMouseOverColor,
	_In_ w_color pButtonPressedColor,
	_In_ w_color pButtonFocusedColor,
	_In_ w_color pButtonDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ const std::string pFontName,
	_In_ UINT pFontSize,
	_In_ UINT pFontWeight,
	_In_ UINT pFontStyle,
	_In_ UINT pFontStretch,
	_In_ UINT pFontParagraphAlignment,
	_In_ UINT pFontTextAlignment)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_slider* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_slider(pID,
			pX, pY,
			pWidth, pHeight,
			pMin, pMax,
			pValue,
			pHotkey,
			pIsDefault,
			pMainColor, pMainMouseOverColor, pMainPressedColor, pMainFocusedColor, pMainDisabledColor,
			pButtonColor, pButtonMouseOverColor, pButtonPressedColor, pButtonFocusedColor, pButtonDisabledColor,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating slider with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_list_box(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ bool pMultiSelection,
	_In_ int pBorderX, _In_ int pBorderY,
	_In_ int pTextMarginX, _In_ int pTextMarginY,
	_In_ int pIconMarginX, _In_ int pIconMarginY,
	_In_ float pIconWidth, _In_ float pIconHeight,
	_In_ bool pIconUseDefaultSize,
	_In_ int pSelectedRectangleMarginTop, _In_ int pSelectedRectangleMarginDown,
	_In_ int pItemHeight,
	_In_ int pIconHeightOffset,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ w_color pTextColor,
	_In_ w_color pSelectedTextColor,
	_In_ w_color pDisabledTextColor,
	_In_ w_color pColor,
	_In_ w_color pSelectedColor,
	_In_ w_color pDisabledColor,
	_In_ w_color pScrollColor,
	_In_ w_color pScrollBackgroundColor,
	_In_ w_color pScrollDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ const std::string pFontName,
	_In_ UINT pFontSize,
	_In_ UINT pFontWeight,
	_In_ UINT pFontStyle,
	_In_ UINT pFontStretch,
	_In_ UINT pFontParagraphAlignment,
	_In_ UINT pFontTextAlignment,
	_In_ std::vector<std::tuple<std::wstring, std::wstring, bool>>& pItems)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_list_box* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_list_box(pID,
			pX, pY,
			pWidth, pHeight,
			pMultiSelection,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating list_box with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_border(pBorderX, pBorderY);
			_control->set_text_margin(pTextMarginX, pTextMarginY);
			_control->set_icon_margin(pIconMarginX, pIconMarginY);
			_control->set_icon_use_default_size(pIconUseDefaultSize);
			_control->set_icon_size(pIconWidth, pIconHeight);
			_control->set_item_height(pItemHeight);
			_control->set_icon_height_offset(pIconHeightOffset);
			_control->set_item_selected_rectangle_margin(pSelectedRectangleMarginTop, pSelectedRectangleMarginDown);
			_control->set_label_color(pTextColor);
			_control->set_label_selected_color(pSelectedTextColor);
			_control->set_label_disabled_color(pDisabledTextColor);
			_control->set_list_color(pColor);
			_control->set_list_selected_color(pSelectedColor);
			_control->set_list_disabled_color(pDisabledColor);
			_control->set_scroll_color(pScrollColor);
			_control->set_scroll_background_color(pScrollBackgroundColor);
			_control->set_scroll_disabled_color(pScrollDisabledColor);
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);

			for (auto _tuple : pItems)
			{
				_control->add_item(
					std::get<0>(_tuple),
					std::get<1>(_tuple),
					std::get<2>(_tuple));
			}
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_list_widget(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ bool pMultiSelection,
	_In_ int pBorderX, _In_ int pBorderY,
	_In_ int pSelectedRectangleMarginTop, _In_ int pSelectedRectangleMarginDown,
	_In_ int pItemHeight,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ w_color pColor,
	_In_ w_color pSelectedColor,
	_In_ w_color pDisabledColor,
	_In_ w_color pScrollColor,
	_In_ w_color pScrollBackgroundColor,
	_In_ w_color pScrollDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_list_widget* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_list_widget(pID,
			pX, pY,
			pWidth, pHeight,
			pMultiSelection,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating list_widget with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_border(pBorderX, pBorderY);
			_control->set_item_height(pItemHeight);
			_control->set_item_selected_rectangle_margin(pSelectedRectangleMarginTop, pSelectedRectangleMarginDown);
			_control->set_list_color(pColor);
			_control->set_list_selected_color(pSelectedColor);
			_control->set_list_disabled_color(pDisabledColor);
			_control->set_scroll_color(pScrollColor);
			_control->set_scroll_background_color(pScrollBackgroundColor);
			_control->set_scroll_disabled_color(pScrollDisabledColor);
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_tab(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ UINT pTabButtonWidth, _In_ UINT pTabButtonHeight,
	_In_ int ppArrowButtonTextOffsetX, _In_ int ppArrowButtonTextOffsetY,
	_In_ w_color pButtonColor,
	_In_ w_color pButtonMouseOverColor,
	_In_ w_color pButtonPressedColor,
	_In_ w_color pButtonFocusedColor,
	_In_ w_color pButtonDisabledColor,
	_In_ w_color pTextColor,
	_In_ w_color pTextMouseOverColor,
	_In_ w_color pTextPressedColor,
	_In_ w_color pTextFocusedColor,
	_In_ w_color pTextDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled,
	_In_ std::vector<std::tuple<std::wstring, std::wstring, bool, glm::vec2, glm::vec2, glm::vec2, int, bool>>& pItems)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_tab* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_tab(pID,
			pX, pY,
			pWidth, pHeight,
			pTabButtonWidth, pTabButtonHeight,
			ppArrowButtonTextOffsetX, ppArrowButtonTextOffsetY,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating tab with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);

			_control->set_button_color(pButtonColor);
			_control->set_button_mouse_over_color(pButtonMouseOverColor);
			_control->set_text_pressed_color(pButtonPressedColor);
			_control->set_button_focused_color(pButtonFocusedColor);
			_control->set_button_disabled_color(pButtonDisabledColor);

			_control->set_button_color(pButtonColor);
			_control->set_button_mouse_over_color(pButtonMouseOverColor);
			_control->set_text_pressed_color(pButtonPressedColor);
			_control->set_button_focused_color(pButtonFocusedColor);
			_control->set_button_disabled_color(pButtonDisabledColor);

			for (auto _tuple : pItems)
			{
				auto _3 = std::get<3>(_tuple);
				auto _4 = std::get<4>(_tuple);
				auto _5 = std::get<5>(_tuple);

				_control->add_tab_item(std::get<0>(_tuple),
					std::get<1>(_tuple),
					std::get<2>(_tuple),
					_3.x, _3.y,
					_4.x, _4.y,
					_5.x, _5.y,
					std::get<6>(_tuple),
					std::get<7>(_tuple));
			}
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_line_shape(_In_ int pID,
	_In_ int pStartX, _In_ int pStartY,
	_In_ int pStopX, _In_ int pStopY,
	_In_ UINT pStrokeWidth,
	_In_ bool pIsDefault,
	_In_ w_color pColor,
	_In_ w_color pDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_line_shape* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_line_shape(pID,
			pStartX, pStartY,
			pStopX, pStopY,
			pStrokeWidth,
			pIsDefault,
			pColor,
			pDisabledColor,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating line shape control with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
		}

		return _hr;
	}
	return S_FALSE;
}

HRESULT w_gui::add_rounded_rectangle_shape(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_In_ float pRadiusX, _In_ float pRadiusY,
	_In_ float pStrokeWidth,
	_In_ bool pIsDefault,
	_In_ w_color pFillColor,
	_In_ w_color pBorderColor,
	_In_ w_color pMouseOverColor,
	_In_ w_color pDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_rounded_rectangle_shape* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_rounded_rectangle_shape(pID,
			pX, pY,
			pWidth, pHeight,
			pRadiusX, pRadiusY,
			pStrokeWidth,
			pIsDefault,
			pFillColor,
			pBorderColor,
			pMouseOverColor,
			pDisabledColor,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating rounded rectangle shape control with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
		}

		return _hr;
	}
	return S_FALSE;
}


HRESULT w_gui::add_ellipse_shape(_In_ int pID,
	_In_ float pCenterX, _In_ float pCenterY,
	_In_ float pRadiusX, _In_ float pRadiusY,
	_In_ UINT pStrokeWidth,
	_In_ bool pIsDefault,
	_In_ w_color pFillColor,
	_In_ w_color pBorderColor,
	_In_ w_color pMouseOverColor,
	_In_ w_color pDisabledColor,
	_In_ float pZOrder,
	_In_ bool pEnabled)
{
	if (_temp_parent_widget_ptr)
	{
		wolf::gui::w_ellipse_shape* _control = nullptr;
		auto _hr = _temp_parent_widget_ptr->add_ellipse_shape(pID,
			pCenterX, pCenterY,
			pRadiusX, pRadiusY,
			pStrokeWidth,
			pIsDefault,
			pFillColor,
			pBorderColor,
			pMouseOverColor,
			pDisabledColor,
			&_control);
		if (FAILED(_hr))
		{
			auto _msg = std::string("creating ellipse shape control with id: ") + std::to_string(pID);
			V(_hr, _msg, _trace_class_name, 3);
			return _hr;
		}

		if (_control)
		{
			_control->set_z_order(pZOrder);
			_control->set_enabled(pEnabled);
		}

		return _hr;
	}
	return S_FALSE;
}

void w_gui::render(const wolf::system::w_game_time& pGameTime)
{
#ifdef WIN32
	//if minimized
	if (IsIconic(_hwnd)) return;
#endif

	auto _time = (float)pGameTime.get_elapsed_seconds();
	std::for_each(_widgets.begin(), _widgets.end(), [&_time](const std::pair<const std::string, wolf::gui::w_widget*> pWidget)
	{
		auto _ptr = pWidget.second;
		if (_ptr)
		{
			_ptr->render(_time);
		}
	});
}

void w_gui::render(const char* pWidgetName, const wolf::system::w_game_time& pGameTime)
{
	auto _find = _widgets.find(pWidgetName);
	if (_find != _widgets.end())
	{
		auto _ptr = _find->second;
		if (_ptr)
		{
			_ptr->render((float)pGameTime.get_elapsed_seconds());
			return;
		}
	}

	auto _msg = std::string("a widget with name: ") + pWidgetName + std::string(" does not exists in gui");
	V(S_FALSE, _msg, _trace_class_name, 2);
}

HRESULT w_gui::release()
{
	if (w_gui::_is_released) return 0;
	
	for (auto _iter = _widgets.begin(); _iter != _widgets.end(); ++_iter)
	{
		if (_iter->second)
		{
			_iter->second->release();
		}
	}
	_widgets.clear();

	_sprite_batch = nullptr;
	if (_widget_resource_manager)
	{
		_widget_resource_manager->release();
	}
	_temp_parent_widget_ptr = nullptr;
	_gDevice = nullptr;
	_is_released = true;

	return 1;
}

#pragma region Getters

void w_gui::get_first_widget(_Inout_ wolf::gui::w_widget** pWidget)
{
	if (_widgets.size() == 0) 
	{
		*pWidget = nullptr;
		return;
	}

	//get first one
	*pWidget = _widgets.begin()->second;
}

void w_gui::get_widget(_In_z_ const char* pWidgetName, _Inout_ wolf::gui::w_widget** pWidget)
{
	auto _find = _widgets.find(pWidgetName);
	if (_find != _widgets.end())
	{
		auto _ptr = _find->second;
		if (_ptr)
		{
			*pWidget = _ptr;
			return;
		}
	}
	*pWidget = nullptr;
}

void w_gui::get_control(_In_z_ const char* pWidgetName, _In_ int pControlID, _Inout_ wolf::gui::w_control** pControl)
{
	auto _find = _widgets.find(pWidgetName);
	if (_find != _widgets.end())
	{
		auto _ptr = _find->second;
		if (_ptr)
		{
			*pControl = _ptr->get_control(pControlID);
			return;
		}
	}
		
	*pControl = nullptr;
	return;
}

void w_gui::get_all_controls_id(_In_z_ const char* pWidgetName, _Inout_ std::vector<int>& pControlsID)
{
	auto _find = _widgets.find(pWidgetName);
	if (_find != _widgets.end())
	{
		auto _ptr = _find->second;
		if (_ptr)
		{
			_ptr->get_all_controls_id(pControlsID);
			return;
		}
	}

	return;
}

#pragma endregion