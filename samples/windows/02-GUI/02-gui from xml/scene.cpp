#include "pch.h"
#include "scene.h"
#include <functional>

using namespace std;
using namespace wolf::graphics;
using namespace wolf::gui;

scene::scene() :
	_widget_1(nullptr),
	_widget_2(nullptr),
	_widget_3(nullptr)
{
	_super::set_app_name(L"02_GUI_from_XML_DX11_X.Win32");
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
	auto _xml_gui_path = wolf::system::io::get_content_directoryW() + L"GUI\\test-01.xml";

	w_gui::initialize(_gDevice, this->sprite_batch.get(), _screen_width, _screen_height);
	auto _hr = w_gui::load(_hwnd, _xml_gui_path.c_str());
	V(_hr, L"loading gui design file", this->name, 3);

	//get all widgets by their ID
	w_gui::get_first_widget(&this->_widget_1);
	w_gui::get_widget("widget_2", &this->_widget_2);
	w_gui::get_widget("widget_3", &this->_widget_3);

	//bind callback events of widget_3 to our code
	if (this->_widget_3)
	{
		//bind the callbacks for mouse over, mouse enter and mouse leave events
		this->_widget_3->on_mouse_move_callback = std::bind(&scene::on_widget_mouse_move_callback, this, std::placeholders::_1);
		this->_widget_3->on_mouse_enter_callback = std::bind(&scene::on_widget_mouse_enter_callback, this, std::placeholders::_1);
		this->_widget_3->on_mouse_leave_callback = std::bind(&scene::on_widget_mouse_leave_callback, this, std::placeholders::_1);
	}
}

void scene::load()
{
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
	//check message procedure of each widget
	if (this->_widget_3 && this->_widget_3->on_msg_proc(pHWND, pMsg, pWParam, pLParam)) return S_OK;
	if (this->_widget_2 && this->_widget_2->on_msg_proc(pHWND, pMsg, pWParam, pLParam)) return S_OK;
	if (this->_widget_1 && this->_widget_1->on_msg_proc(pHWND, pMsg, pWParam, pLParam)) return S_OK;
	
	return S_FALSE;
}

ULONG scene::release()
{
	if (this->is_released()) return 0;
	
	//release all resources

	this->_widget_1 = nullptr;
	this->_widget_2 = nullptr;
	this->_widget_3 = nullptr;

	w_gui::release();

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
	this->_widget_3->set_background_colors(
		w_color(216, 238, 249, 255),
		w_color(216, 238, 249, 255),
		w_color(71, 188, 242, 255),
		w_color(71, 188, 242, 255));
}

void scene::on_widget_mouse_leave_callback(const POINT& pMousePoint)
{
	//on mouse leaves the widget, change the color of widget
	this->_widget_3->set_background_colors(
		w_color(216, 238, 249, 100),
		w_color(216, 238, 249, 100),
		w_color(71, 188, 242, 100),
		w_color(71, 188, 242, 100));
}

#pragma endregion
