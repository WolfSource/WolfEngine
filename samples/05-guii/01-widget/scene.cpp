#include "pch.h"
#include "scene.h"
#include <functional>

using namespace std;
using namespace wolf::graphics;
using namespace wolf::gui;

scene::scene() :
	_widget(nullptr)
{
	_super::set_app_name(L"01_Widget_DX11_X.Win32");
}

scene::~scene()
{
	release();
}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	_super::initialize(pOutputWindowsInfo);

	// TODO: Add your post-initialization logic here


	/*
		_super::initialize(pOutputWindowsInfo) will create graphics devices,
		so make sure call get_graphics_device after _super::initialize
	*/
	auto _gDevice = get_graphics_device();

	//get the window informations, such as width, height and hwnd
	auto _screen_width = _super::get_window_width();
	auto _screen_height = _super::get_window_height();
	auto _hwnd = _super::get_window_HWND();

	//initialize the gui
	w_gui::initialize(_gDevice, this->sprite_batch.get(), _screen_width, _screen_height);
	//programmatically add a widget
	w_gui::add_widget(
		_hwnd,
		"widget_1",
		&this->_widget,
		10,10,
		400, 400,
		true,
		true,
		false,
		w_color(216, 238, 249, 100), w_color(216, 238, 249, 100), w_color(71, 188, 242, 100), w_color(71, 188, 242, 100),
		w_color(216, 238, 249, 255), w_color(216, 238, 249, 255), w_color(71, 188, 242, 255), w_color(71, 188, 242, 255),
		0,
		true,
		true,
		L"Drag or double click on me",
		w_color(57, 57, 57, 150),
		w_color(57, 57, 57, 255));

	//if widget created, then register callback events
	if (this->_widget)
	{
		//bind the callbacks for mouse over, mouse enter and mouse leave events
		this->_widget->on_mouse_move_callback = std::bind(&scene::on_widget_mouse_move_callback, this, std::placeholders::_1);
		this->_widget->on_mouse_enter_callback = std::bind(&scene::on_widget_mouse_enter_callback, this, std::placeholders::_1);
		this->_widget->on_mouse_leave_callback = std::bind(&scene::on_widget_mouse_leave_callback, this, std::placeholders::_1);
	}
}

void scene::load()
{
	// TODO: load your game assets here

	_super::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
	_super::update(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	//draw gui controls
	w_gui::render(pGameTime);
	
	_super::render(pGameTime);
}

HRESULT scene::on_msg_proc(HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	//check message procedure of widget
	if (this->_widget && this->_widget->on_msg_proc(pHWND, pMsg, pWParam, pLParam)) return S_OK;

	return S_FALSE;
}

ULONG scene::release()
{
	if (this->is_released()) return 0;
	
	//release all resources
	w_gui::release();
	this->_widget = nullptr;

	return _super::release();
}

#pragma region events

void scene::on_widget_mouse_move_callback(const POINT& pMousePoint)
{
	//store mouse position in log file and output window of visula studio, when mouse moves over the widget
	logger.write(std::to_string(pMousePoint.x));
	logger.write(std::to_string(pMousePoint.y));
}

void scene::on_widget_mouse_enter_callback(const POINT& pMousePoint)
{
	//on mouse enters the widget, change the color of widget 
	logger.write("Mouse enter over widget");
}

void scene::on_widget_mouse_leave_callback(const POINT& pMousePoint)
{
	//on mouse leaves the widget, change the color of widget
	logger.write("Mouse leaves widget");
}

#pragma endregion
