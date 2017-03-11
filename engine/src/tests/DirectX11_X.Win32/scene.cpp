/*
Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to contact@WolfSource.io or tweet @Wolf_Engine on twitter
Name			 : Scene.cpp
Description		 : This is simple test, you can remove this project, during build
Comment          :
*/

#include "pch.h"
#include "Scene.h"
#include <w_time_span.h>
#include <w_ffmpeg.h>
#include <future>
#include <w_graphics\w_textures\ScreenGrab.h>
#include <w_memory.h>
#include <w_xml.h>
#include <w_timer.h>
#include <w_gui/w_user_controls/w_label.h>
#include <w_gui/w_user_controls/w_combo_box.h>
#include <w_gui/w_user_controls/w_slider.h>
#include <w_gui/w_user_controls/w_button.h>
#include <w_gui/w_user_controls/w_tree_list.h>
#include <w_gui/w_user_controls/w_list_widget.h>
#include <w_xml.h>
#include <decklink.h>
#include <w_python.h>
#include <boost/filesystem.hpp>
#include <w_content_manager.h>
#include <w_scene.h>
#include <w_camera.h>
//#include <w_tcp_client.h>

#include <w_timer_callback.h>

#include <decklink.h>
//static std::unique_ptr<decklink>	sDeckLink;

#include <w_lua.h>
#include <w_task.h>

#include <string>
#include <locale>
#include <codecvt>

#pragma comment(lib, "freetype271MTd.lib")
#include <ft2build.h>
#include <freetype/freetype.h>
FT_Library _freetype_lib;
FT_Face _freetype_face;

using namespace std;
using namespace DirectX;
using namespace D2D1;
using namespace wolf::system;
using namespace wolf::framework;
using namespace wolf::graphics;
using namespace wolf::graphics::directX;
using namespace wolf::graphics::direct2D::shapes;

#define W_GUI_ID_LABEL						0x000000
#define W_GUI_ID_BUTTON						0x000001
#define W_GUI_ID_CHECKBOX					0x000002
#define W_GUI_ID_COMBOBOX					0x000003
#define W_GUI_ID_RADIOBUTTON_1				0x000004
#define W_GUI_ID_RADIOBUTTON_2				0x000005
#define W_GUI_ID_RADIOBUTTON_3				0x000006
#define W_GUI_ID_SLIDER_1					0x000007
#define W_GUI_ID_SLIDER_2					0x000008
#define W_GUI_ID_SLIDER_3					0x000009
#define W_GUI_ID_TAB_1						0x00000A

static void CALLBACK on_gui_event(UINT pEvent, int pControlID, wolf::gui::w_control* pControl, void* pUserContext);
static wolf::content_pipeline::w_camera* camera;
//static wolf::system::network::w_tcp_client* c;

//static ID2D1PathGeometry1* sPathGeometry = nullptr;
//static ID2D1SolidColorBrush* sBrush = nullptr;
//static w_sprite_font* sFont;
//static w_sprite_font* sDrawFont;
//static std::wstring sText = L"this is test. این تست است";

static int sum(int pX, int pY)
{
	return pX + pY;
}

static int lua_bind_sum(lua_State* pLua)
{
	int _x = lua_tointeger(pLua, 1);
	int _y = lua_tointeger(pLua, 2); // get function arguments
	lua_pushnumber(pLua, sum(_x, _y));  // push the result of a call
	return 1;// we're returning one result
}

scene::scene() :
	_widget(nullptr)
{
	w_game::set_app_name(L"Wolf.TestDX.Win32");
	w_game::set_fixed_time_step(false);

	ColorF color = ColorF::Navy;
	this->clear_color[0] = color.r;
	this->clear_color[1] = color.g;
	this->clear_color[2] = color.b;






	//int _x = -1;
	//double _var1;
	//std::wstring _var2;
	//w_lua::load_file(L"C:\\Users\\PooyaEimandar\\Desktop\\test_lua\\x64\\Debug\\test.lua");
	//w_lua::bind_to_cfunction(lua_bind_sum, "sum_cpp");
	//w_lua::run();
	//w_lua::set_global_variable("var1", 3.1);
	//w_lua::set_global_variable("var2", L"سلام");
	//w_lua::get_global_variable("var1", _var1);
	//w_lua::get_global_variable("var2", _var2);

	//w_lua::get_global_variable("x", _x);

	//w_lua::prepare_function("update");
	//w_lua::set_parameter_function(2);
	//w_lua::set_parameter_function(9);

	//int _y = 0;
	//std::string _z = "";
	//w_lua::execute_function(_x, _y, _z);

	//logger.write(std::to_string(_x));
	//logger.write(std::to_string(_y));
	//logger.write(_z);


	//std::function<int(int,int)> _func = std::bind(&sum, std::placeholders::_1, std::placeholders::_2);

	//int c = _func(10, 20);
	//logger.write(std::to_string(c));


	//w_lua::release();

	//logger.write(_var);

	//c = new wolf::system::network::w_tcp_client(service, "127.0.0.1", 10540); //192.168.120.110

	//c->register_on_connect_callback(boost::bind(&scene::_on_connect, this, boost::asio::placeholders::error));
	//
	//c->register_on_sent_callback(boost::bind(
	//	&scene::_on_sent, 
	//	this, boost::asio::placeholders::error, 
	//	boost::asio::placeholders::bytes_transferred));

	//c->register_on_received_callback(boost::bind(
	//	&scene::_on_received, 
	//	this, boost::asio::placeholders::error, 
	//	boost::asio::placeholders::bytes_transferred, 
	//	boost::asio::placeholders::detail::placeholder<3>::get()));

	//c->async_connect();

	//c->connect();



	//auto _write = c->write("-r>10>11>Program-CG#EOF#\r\n");
	//logger.write(std::to_string(_write));
	//std::string _msg;
	//auto _read = c->read(_msg);
	//logger.write(_msg);

	//_write = c->write("-ping#EOF#\r\n");
	//_read = c->read(_msg);
	//logger.write(_msg);

	//_write = c->write("-quit#EOF#\r\n");
	//_read = c->read(_msg);
	//logger.write(_msg);

	//c->close();
	//delete c;
	//_scene->load();

	//auto _t = w_time_span::from_string("01:10:20:33:456");
	//boost::filesystem::path p("E:\\B.dds");
	//logger.write(p.extension().c_str());

	//sDeckLink = std::make_unique<decklink>();
	/*auto _d = new decklink();
	_d->initialize();
	*/
	//wolf::system::w_python _p;
	////p.register_class_methods("CppClass", boost::python::class_<CppClass>("CppClass").
	////	def("getNum", &CppClass::getNum).
	////	def("setNum", &CppClass::setNum));
	////
	////CppClass cpp;
	////p.register_pointer_class("cpp", boost::python::ptr(&cpp));

	//_p.run("result = 1*1 \n");

	//auto _r = _p.get_result<int>("result");
	//logger.write(std::to_string(_r));

	//_p.release();	



}

scene::~scene()
{
	release();
}

//void scene::_on_connect(const boost::system::error_code& pErrorCode)
//{
//	logger.write("A");
//}

//void scene::_on_sent(const boost::system::error_code& pErrorCode, const size_t pBytesTransferred)
//{
//	logger.write(std::to_string(pBytesTransferred));
//	if (!pErrorCode)
//	{
//		//c->async_read();
//	}
//}

//void scene::_on_received(const boost::system::error_code& pErrorCode, const size_t pBytesTransferred, std::string _pReceivedMessage)
//{
//	logger.write(_pReceivedMessage);
//}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	w_game::initialize(pOutputWindowsInfo);
}

//static W_Memory videoMemory;
//static W_Memory audioMemory;

static w_timer timer;
static w_renderable_scene* sScene;
static wolf::gui::w_tree_list* _tree_list = nullptr;
static wolf::gui::w_list_widget* _list_widget = nullptr;

static std::vector<wolf::graphics::w_quad<>*> _text_quads;
void scene::load()
{
	w_game::load();

	auto _gDevice = get_graphics_device();
	auto _width = this->get_window_width();
	auto _height = this->get_window_height();

	auto _hwnd = _gDevice->output_windows[0].hwnd;


	auto _error = FT_Init_FreeType(&_freetype_lib);
	if (_error)
	{
		logger.write("Free type error");
	}
	_error = FT_New_Face(_freetype_lib, "C:\\Users\\PooyaEimandar\\Documents\\github\\Wolf.Engine\\bin\\x64\\Debug\\Win32\\arial.ttf", 0, &_freetype_face);
	if (_error == FT_Err_Unknown_File_Format)
	{
		logger.write("FT_Err_Unknown_File_Format");
	}
	else if (_error)
	{
		logger.write("error FT_New_Face");
	}


	_error = FT_Set_Char_Size(_freetype_face, 0, 1024 * 1024, 300, 300);
	if (_error)
	{
		logger.write("error FT_Set_Char_Size");
	}

	_error = FT_Set_Pixel_Sizes(_freetype_face, 0, 1024);
	if (_error)
	{
		logger.write("error FT_Set_Pixel_Sizes");
	}

	_error = FT_Select_Charmap(_freetype_face, ft_encoding_unicode);
	if (_error)
	{
		logger.write("error FT_Select_Charmap");
	}

	FT_GlyphSlot _glyph = _freetype_face->glyph;

	//(LC_ALL, "fa-IR");
	std::wstring _text = L"بآ";

	auto _lenght = _text.size();
	auto _char_codes = new FT_ULong(_lenght);
	for (size_t i = 0; i < _lenght; i++)
	{
		auto _typed_char_code = (FT_ULong)_text[i];
		_char_codes[i] = _typed_char_code;

		if (_typed_char_code == 0x0622)//آ
		{
			if (i == 0)
			{
				_char_codes[i] = 0xFE82;
			}
			else
			{
				auto _pr_char_code = _char_codes[i - 1];
				logger.write(std::to_string(_pr_char_code));
				if (_pr_char_code == 0xFB58 ||
					_pr_char_code == 0xFB59 ||
					_pr_char_code == 0xFB6C ||
					_pr_char_code == 0xFB6D ||
					_pr_char_code == 0xFB7C ||
					_pr_char_code == 0xFB7D ||
					_pr_char_code == 0xFB90 ||
					_pr_char_code == 0xFB91 ||
					_pr_char_code == 0xFB94 ||
					_pr_char_code == 0xFB95 ||
					_pr_char_code == 0xFBFE ||
					_pr_char_code == 0xFBFF ||
					_pr_char_code == 0xFE91 ||
					_pr_char_code == 0xFE92 ||
					_pr_char_code == 0xFE97 ||
					_pr_char_code == 0xFE98 ||
					_pr_char_code == 0xFE9B ||
					_pr_char_code == 0xFE9C ||
					_pr_char_code == 0xFE9F ||
					_pr_char_code == 0xFEA0 ||
					_pr_char_code == 0xFEA3 ||
					_pr_char_code == 0xFEA4 ||
					_pr_char_code == 0xFEA7 ||
					_pr_char_code == 0xFEA8 ||
					_pr_char_code == 0xFEA9 ||
					_pr_char_code == 0xFEAA ||
					_pr_char_code == 0xFEAC ||
					_pr_char_code == 0xFEB3 ||
					_pr_char_code == 0xFEB4 ||
					_pr_char_code == 0xFEB7 ||
					_pr_char_code == 0xFEB8 ||
					_pr_char_code == 0xFEBB ||
					_pr_char_code == 0xFEBC ||
					_pr_char_code == 0xFEBF ||
					_pr_char_code == 0xFEC0 ||
					_pr_char_code == 0xFEC2 ||
					_pr_char_code == 0xFEC3 ||
					_pr_char_code == 0xFEC4 ||
					_pr_char_code == 0xFEC6 ||
					_pr_char_code == 0xFEC7 ||
					_pr_char_code == 0xFEC8 ||
					_pr_char_code == 0xFECB ||
					_pr_char_code == 0xFECC ||
					_pr_char_code == 0xFECF ||
					_pr_char_code == 0xFED0 ||
					_pr_char_code == 0xFED3 ||
					_pr_char_code == 0xFED4 ||
					_pr_char_code == 0xFED7 ||
					_pr_char_code == 0xFED8 ||
					_pr_char_code == 0xFEDB ||
					_pr_char_code == 0xFEDC ||
					_pr_char_code == 0xFEDF ||
					_pr_char_code == 0xFEE0 ||
					_pr_char_code == 0xFEE4 ||
					_pr_char_code == 0xFEE7 ||
					_pr_char_code == 0xFEE8 ||
					_pr_char_code == 0xFEF3)
				{
					_char_codes[i] = 0xFE82;
				}
			}
		}
		else if (_typed_char_code == 0x0628)//ب
		{
			if (i == 0)
			{
				_char_codes[i] = 0xFE91;
			}
		}
	
		_error = FT_Load_Char(_freetype_face, _char_codes[i], FT_LOAD_RENDER);
		if (_error) continue;

		/*auto _d = new uint8_t[_slot->bitmap.width * _slot->bitmap.rows * 4];
		for (size_t i = 0, j = 0; i < _slot->bitmap.width * _slot->bitmap.rows; i++, j += 4)
		{
		_d[j] = _slot->bitmap.buffer[i];
		_d[j + 1] = 0;
		_d[j + 2] = 0;
		_d[j + 3] = 255;
		}*/


		auto __width = _glyph->bitmap.width;
		auto __height = _glyph->bitmap.rows;
		if (__width == 0 || __height == 0)
		{
			continue;
		}

		D3D11_SUBRESOURCE_DATA _init_data;
		std::memset(&_init_data, 0, sizeof(_init_data));
		_init_data.pSysMem = (void*)_glyph->bitmap.buffer;
		_init_data.SysMemPitch = static_cast<UINT>(__width);// *4);
		_init_data.SysMemSlicePitch = static_cast<UINT>(__width * __height);// *4);


		//wolf::graphics::w_texture_2D _t;
		//_t.create(_gDevice->device.Get(),
		//	static_cast<UINT>(_width),
		//	static_cast<UINT>(_height),
		//	D3D11_USAGE_DYNAMIC,
		//	D3D11_CPU_ACCESS_WRITE,
		//	DXGI_FORMAT_R8_UNORM,
		//	D3D11_BIND_SHADER_RESOURCE,
		//	&_init_data);

		//auto _desc = _t.get_description();

		auto _text_quad = new wolf::graphics::w_quad<>();
		_text_quad->load(_gDevice->device.Get());
		_text_quad->create_texture_2D(_gDevice->device.Get(),
			__width,
			__height,
			D3D11_USAGE_DEFAULT,
			D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
			DXGI_FORMAT::DXGI_FORMAT_R8_UNORM,
			D3D11_BIND_SHADER_RESOURCE,
			&_init_data);

		_text_quad->set_scale((float)__width / 1024.0f, (float)__height / 1024.0f, 1.0f);

		_text_quads.push_back(_text_quad);
		//SaveDDSTextureToFile(_gDevice->context.Get(), _t.get_resource(), L"C:\\P\\A.dds");
	}


	//wolf::content_pipeline::w_content_manager _c;
	//auto _scene = _c.load<wolf::content_pipeline::w_scene>(L"H:\\Codes\\Poser\\Cube.dae");
	//sScene = new w_renderable_scene(_scene);
	//sScene->load(_gDevice);
	//sScene->get_first_or_default_camera(&camera);

	camera = new wolf::content_pipeline::w_camera();
	camera->set_aspect_ratio(static_cast<float>(this->get_window_width() / this->get_window_height()));
	camera->set_transform(0, 0, 2.0f);
	camera->update_view();
	camera->update_projection();

	wolf::gui::w_gui::initialize(_gDevice, sprite_batch.get(), _width, _height);
	auto _gui_path = wolf::system::io::get_content_directoryW() + L"GUI\\test-02.xml";
	wolf::gui::w_gui::load(_hwnd, _gui_path.c_str());

	wolf::gui::w_gui::get_widget("widget_2", &this->_widget);
	if (this->_widget)
	{
		std::vector<int> _v;
		this->_widget->get_all_controls_id(_v);
		logger.write(std::to_string(_v.size()));
		using namespace wolf::gui;

		//this->_widget->add_list_widget(100, 5, 5, 390, 550, false, &_list_widget);
		//if (_list_widget)
		//{
		//	_list_widget->set_item_height(190);
		//	_list_widget->set_item_selected_rectangle_margin(4, 170);
		//	
		//	auto _list_widget_id = _list_widget->get_ID();
		//	for (size_t i = 0; i < 20; i++)
		//	{
		//		_list_widget_id += 1;
		//		auto _item = new w_list_widget_item();
		//		if (i < 10)
		//		{
		//			int _x = 100, _y = 25;

		//			auto _check_box = new wolf::gui::w_check_box(this->_widget);
		//			_check_box->set_ID(_list_widget_id);
		//			_check_box->set_text(L"AAAAA");
		//			_check_box->set_checked(true);
		//			_check_box->set_size(150,30);
		//			_check_box->set_text_offset_x(-27);
		//			_check_box->set_text_offset_y(10);
		//			_check_box->set_enabled(true);
		//			_check_box->set_visible(true);

		//			_item->gpu_controls.push_back(std::make_tuple(_check_box, _x, _y));

		//			//_x = 50;

		//			auto _label = new w_label(this->_widget);
		//			_label->set_ID(_list_widget_id);
		//			_label->set_text(L"Hello");
		//			_label->set_size(100, 20);
		//			_label->set_label_color(w_color(0, 0, 0, 255));
		//			_label->set_enabled(true);
		//			_label->set_visible(true);

		//			_item->gpu_controls.push_back(std::make_tuple(_label, _x, _y));

		//			_x = 100;
		//			_list_widget_id += 1;
		//			auto _button = new w_button(this->_widget);
		//			_button->set_ID(_list_widget_id);
		//			_button->set_text(L"Button");
		//			_button->set_size(100, 25);
		//			_button->set_enabled(true);
		//			_button->set_visible(true);

		//			_item->gpu_controls.push_back(std::make_tuple(_button, _x, _y));
		//		}

		//	//	_item->gdi_controls.push_back(sTextBoxesHandles[i]);
		//		_list_widget->add_item(_item);
		//	}
		//}

		this->_widget->add_tree_list(100, 300, 100, 390, 550, false, &_tree_list);
		if (_tree_list)
		{
			auto _list_box_id = _tree_list->get_ID();
			_tree_list->add_item(L"Family", { L"Pooya", L"Ray" });// ,
																  //L"C:\\Users\\Pooya's MAC\\Desktop\\CG\\Wolf.Engine\\bin\\x64\\Release\\Win32\\Content\\Textures\\WolfEngine.dds",
																  //true);


			_tree_list->add_item(L"Family", { L"Pooya", L"Ray" });
		}
		//this->_widget->add_line_shape(0, 600, 100, 680, 130, 10, false, w_color(0,255,0,255), w_color(0, 0, 0, 255));
		//this->_widget->add_rounded_rectangle_shape(0, 20, 20, 100, 150, 20, 20, 10);
		//this->_widget->add_ellipse_shape(0, 600, 100, 50, 50, 10, false, w_color(0, 255, 0, 255), w_color(0, 0, 0, 255));

		//auto _tab = reinterpret_cast<wolf::gui::w_tab*>(this->_widget->get_control(0x00000080));
		//if (_tab)
		//{
		//	//_tab->set_call_back(on_gui_event);
		//}
		//wolf::gui::w_tab* _tab = nullptr;
		//this->_widget->add_tab(0, 500, 100, 5, 5, 70, 20, POINT(8, 0), &_tab);
		//if (_tab)
		//{
		//	_tab->add_tab(L"س");
		//	_tab->add_tab(L"ل");
		//	_tab->add_tab(L"ا");
		//	_tab->add_tab(L"p");
		//	_tab->add_tab(L"o");
		//	_tab->add_tab(L"o");
		//	_tab->add_tab(L"y");
		//	_tab->add_tab(L"a");

		//	this->_widget->set_call_back(on_gui_event);
		//}
	}

	// Create path geometry

	//auto _hr = _gDevice->factory_2D->CreatePathGeometry(&sPathGeometry);
	//if (FAILED(_hr))
	//{
	//	MessageBox(NULL, L"Create path geometry failed!", L"Error", 0);
	//	return;
	//}

	//// Open sink
	//ID2D1GeometrySink* _geometry_sink = nullptr;
	//_hr = sPathGeometry->Open(&_geometry_sink);
	//if (FAILED(_hr))
	//{
	//	MessageBox(NULL, L"Open geometry sink failed!", L"Error", 0);
	//	return;
	//}

	//w_sprite_font_style _style_source =
	//{
	//	L"Calibri",
	//	DWRITE_FONT_WEIGHT_NORMAL,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	DWRITE_TEXT_ALIGNMENT_LEADING,
	//	DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
	//	30.0,
	//	L"en-US"
	//};

	//sFont = new w_sprite_font(_style_source);
	//sFont->load(_gDevice);

	//w_sprite_font_style _style_alalam =
	//{
	//	L"AlAlamB",
	//	DWRITE_FONT_WEIGHT_NORMAL,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	DWRITE_TEXT_ALIGNMENT_LEADING,
	//	DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
	//	30.0,
	//	L"en-US"
	//};
	//
	//sDrawFont = new w_sprite_font(_style_alalam);
	//sDrawFont->load(_gDevice);

	//auto _font_face = sFont->get_font_face();

	//// Get text length
	//UINT32 _length = (UINT32)sText.length();// (UINT32)wcslen(text);

	//UINT16* pGlyphIndices = new UINT16[_length];
	//ZeroMemory(pGlyphIndices, sizeof(UINT16) * _length);

	//// Get glyph run outline
	//_hr = _font_face->GetGlyphRunOutline(
	//	sFont->get_font_size(),                              // font size
	//	pGlyphIndices,
	//	NULL,
	//	NULL,
	//	_length,//text length
	//	FALSE,
	//	sFont->get_right_to_left(), //right to left
	//	_geometry_sink);

	//if (FAILED(_hr))
	//{
	//	MessageBox(NULL, L"Get glyph run outline failed!", L"Error", 0);
	//	return;
	//}

	//// Close sink
	//_geometry_sink->Close();


	//	auto _texture = std::make_unique<wolf::graphics::w_texture_2D>();
	//	wolf::graphics::w_texture_2D::load_texture_2D_from_file(_gDevice->device.Get(), _texture, L"c:\\wolfsource.png", true, false);

	//	SaveDDSTextureToFile(_gDevice->context.Get(), _texture->get_resource(), L"d:\\A.dds");

	//auto _hr = wolf::gui::w_gui::create_widget(_gDevice, this->sprite_batch.get(), _hwnd, "widget_0", _width, _height, &this->_widget);
	//this->_widget->set_background_colors(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	//this->_widget->set_callBack(on_gui_event);


	/*for (size_t i = 0; i < 1000; i++)
	{
	w_texture_2D t0;
	timer.start();
	{
	t0.create(
	_gDevice->device.Get(),
	1920,
	1080,
	ColorF::Red,
	D3D11_USAGE_DEFAULT,
	D3D11_CPU_ACCESS_WRITE,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
	GPGPU_TYPE::CPP_AMP);
	}
	timer.stop();

	logger.write("C++ amp : " + std::to_string(timer.getTotalMilliSeconds()));

	DirectX::SaveDDSTextureToFile(_gDevice->context.Get(), t0.get_resource(), L"D:\\t0.dds");

	w_texture_2D t1;


	t1.create(
	_gDevice->device.Get(),
	1920,
	1080,
	ColorF::Green,
	D3D11_USAGE_DYNAMIC,
	D3D11_CPU_ACCESS_WRITE,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	D3D11_BIND_SHADER_RESOURCE,
	GPGPU_TYPE::NONE);

	timer.start();
	{
	D3D11_MAPPED_SUBRESOURCE _mapsub;
	ZeroMemory(&_mapsub, sizeof(D3D11_MAPPED_SUBRESOURCE));

	auto _texture_resource = t1.get_resource();

	auto _hr = _gDevice->context->Map(_texture_resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mapsub);
	auto _data = reinterpret_cast<uint8_t*>(_mapsub.pData);
	_gDevice->context->Unmap(_texture_resource, 0);

	_data = nullptr;

	}
	timer.stop();

	logger.write("C++: " + std::to_string(timer.getElapsedMilliSeconds()));

	DirectX::SaveDDSTextureToFile(_gDevice->context.Get(), t1.get_resource(), L"D:\\t1.dds");

	logger.write("done");
	}*/
	//w_texture_2D t2;

	//timer.start();
	//{
	//	t2.create(
	//		_gDevice->device.Get(),
	//		720,
	//		576,
	//		ColorF(D2D1::ColorF::Red),
	//		D3D11_USAGE_DEFAULT,
	//		D3D11_CPU_ACCESS_WRITE,
	//		DXGI_FORMAT_R8G8B8A8_UNORM,
	//		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE);
	//}
	//timer.Stop();

	//Logger.Write("t2: " + std::to_string(timer.GetTotalMilliSeconds()));

	//DirectX::SaveDDSTextureToFile(_gDevice->context.Get(), t2.GetResource(), L"D:\\t2.dds");

	//Logger.Write("End");
	//W_XML_Data _root;
	//_root.node = L"Playlist";
	//_root.attributes = { std::make_tuple(L"ConductorID", L"?"), std::make_tuple(L"Copyright", L"ALALAM") };

	//W_XML_Data _item_0;
	//_item_0.node = L"Item";
	//_item_0.attributes = 
	//{ 
	//	std::make_tuple(L"Priority", L"1"), 
	//	std::make_tuple(L"Id", L"?"), 
	//	std::make_tuple(L"Type", L"Video"), 
	//	std::make_tuple(L"FilePath", L"D:\Vids\HD0.avi"), 
	//	std::make_tuple(L"Title", L"تست")
	//};
	//
	//W_XML_Data _item_1;
	//_item_1.node = L"Item";
	//_item_1.attributes =
	//{
	//	std::make_tuple(L"Priority", L"2"),
	//	std::make_tuple(L"Id", L"?"),
	//	std::make_tuple(L"Type", L"Video"),
	//	std::make_tuple(L"FilePath", L"D:\Vids\HD1.avi"),
	//	std::make_tuple(L"Title", L"test1")
	//};

	//_root.children = { _item_0, _item_1 };

	//W_XML xml;
	//xml.Save(L"D:\\Vids\\تست\\ت.txt", true, _root);

	//// TODO: load your game assets here
	//auto gDevice = this->GDevice();

	//this->spriteFont = make_unique<W_SpriteFont>(gDevice);
	//this->spriteFont->Load();

	////Load ellipse
	//this->ellipse = make_unique<W_Ellipse>(gDevice);
	//this->ellipse->SetGeormetry(530, 100, 70, 70);
	//this->ellipse->SetColor(ColorF::White);
	//this->ellipse->SetBorderColor(ColorF::White);

	////Load rectangle
	//this->rectangle = make_unique<W_Rectangle>(gDevice);
	//this->rectangle->SetGeormetry(0, 300, this->GetWindowWidth(), 400, 0, 0);
	//this->rectangle->SetColor(ColorF::Blue);
	//this->rectangle->SetBorderColor(ColorF::Blue);

	//auto f = std::async(std::launch::async, []() -> HRESULT
	//{

	/*w_ffmpeg::initialize_MF();
	wstring str = L"G:\\Video Samples\\Beirut 16-11-04.mp4";
	double _db = 0;

	static w_memory b;
	b.malloc(w_memory::_1GB());

	int size = 1000000;

	for (size_t i = 0; i < 1; i++)
	{
	auto f = new w_ffmpeg();

	auto _hr = f->open_media(str);
	for (size_t i = 0; i < size; i++)
	{
	b.clear();
	f->buffer_video_to_memory(b);
	}

	f->release();

	delete f;
	}
	w_ffmpeg::release_MF();

	logger.write("Finished");*/
	//this->quad = make_unique<W_Quad>(gDevice);
	//auto hr = this->quad->Load(false);
	//this->quad->LoadDeafultTexture(720, 576, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	//OnFailed(hr, "Loading quad", this->Name);



	//this->_curve->set_color(ColorF::LightGray);
	//this->_curve->set_fill_geometry(false);

	////this->geoCloud0 = make_unique<W_Geometry>(gDevice);
	////this->geoCloud0->SetColor(ColorF::LightGray);
	////this->geoCloud0->SetBorderColor(ColorF::LightGray);

	//std::array<D2D1_POINT_2F, 3> _array_points;
	//_array_points[0] = D2D1::Point2F(100, 100);
	//_array_points[1] = D2D1::Point2F(150, 50);
	//_array_points[2] = D2D1::Point2F(200, 100);

	//std::vector<std::array<D2D1_POINT_2F, 3>> _points;
	//_points.push_back(_array_points);

	//this->_curve->begin();
	//{
	//	this->_curve->add_beziers(D2D1::Point2F(100, 100), _points, 
	//		D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING,
	//		D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED,
	//		D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);
	//}
	//this->_curve->end();
	//this->geoCloud0->Begin();
	//{
	//	this->geoCloud0->AddArc(
	//		Point2F(40, 220),//Start
	//		D2D1::Point2F(210, 220),//End
	//		D2D1::SizeF(70, 70),//Size
	//		0.0f, //Rotation angle
	//		D2D1_SWEEP_DIRECTION_CLOCKWISE,
	//		D2D1_ARC_SIZE_SMALL);

	//	this->geoCloud0->AddArc(
	//		Point2F(120, 220),//Start
	//		D2D1::Point2F(340, 220),//End
	//		D2D1::SizeF(100, 150),//Size
	//		0.0f, //Rotation angle
	//		D2D1_SWEEP_DIRECTION_CLOCKWISE,
	//		D2D1_ARC_SIZE_SMALL);

	//	this->geoCloud0->AddArc(
	//		Point2F(150, 220),//Start
	//		D2D1::Point2F(400, 220),//End
	//		D2D1::SizeF(100, 90),//Size
	//		0.0f, //Rotation angle
	//		D2D1_SWEEP_DIRECTION_CLOCKWISE,
	//		D2D1_ARC_SIZE_SMALL);
	//}
	//this->geoCloud0->End();

	//this->geoCloud1 = make_unique<W_Geometry>(gDevice);
	//this->geoCloud1->SetColor(ColorF::LightGray);
	//this->geoCloud1->SetBorderColor(ColorF::LightGray);
	//this->geoCloud1->Begin();
	//{
	//	this->geoCloud1->AddArc(
	//		Point2F(420, 220),//Start
	//		D2D1::Point2F(580, 220),//End
	//		D2D1::SizeF(20, 20),//Size
	//		0.0f, //Rotation angle
	//		D2D1_SWEEP_DIRECTION_CLOCKWISE,
	//		D2D1_ARC_SIZE_SMALL);

	//	this->geoCloud1->AddArc(
	//		Point2F(520, 220),//Start
	//		D2D1::Point2F(770, 220),//End
	//		D2D1::SizeF(40, 50),//Size
	//		0.0f, //Rotation angle
	//		D2D1_SWEEP_DIRECTION_CLOCKWISE,
	//		D2D1_ARC_SIZE_SMALL);
	//}
	//this->geoCloud1->End();

	//this->renderTarget2D = make_unique<W_RenderTarget2D>(gDevice);
	//hr = this->renderTarget2D->Load(800, 600);


	////Create texture
	//const UINT width = 512;
	//const UINT height = 512;

	////create a texture from Red color
	//this->texture2D = make_unique<W_Texture2D>();

	////auto hr = this->texture2D->Create(gDevice->device.Get(), width, height, ColorF::White);
	//hr = W_Texture2D::LoadTexture(gDevice->device.Get(), this->texture2D.get(), L"C:\\Test\\t.dds", true);

	////hr = this->texture2D->CreateColorBar(gDevice->device.Get(), width, height);
	//if (hr != S_OK)
	//{
	//	return;
	//}

	//SaveDDSTextureToFile(gDevice->context.Get(), this->texture2D->GetResource(), L"C:\\Test\\AB.dds");


	//gDevice = nullptr;



	//wolf::gui::w_button* _w = nullptr;
	//this->_widget->add_button(W_GUI_ID_BUTTON, L"OK", -300, 400, 64, 64, L"Textures\\Icons\\StopPressed.png", -10, -10, 10, 10, 1.0f, 1.0f, 0, false, &_w);
	//	_w->set_button_color(W_COLOR(255, 0, 0, 255));
	//int x = 10;
	//void* _pointer = &x;
	//_w->set_tag(_pointer);

	//auto _tag = _w->get_tag();
	//logger.write(std::to_wstring(*(int*)_tag));

	//this->_widget->add_image(W_GUI_ID_BUTTON, 0, 0, L"Textures\\Icons\\StopPressed.png");

	//this->_widget->add_label(W_GUI_ID_LABEL, L"Label", -300, 500, 64, 10);
	//this->_widget->add_check_box(W_GUI_ID_CHECKBOX, L"Checkbox", -300, 550, 170, 22);

	//this->_widget->add_radioButton(W_GUI_ID_RADIOBUTTON_1, 0, L"RadioButton 1", -300, 0, 170, 22);
	//this->_widget->add_radioButton(W_GUI_ID_RADIOBUTTON_2, 0, L"RadioButton 2", -300, 30, 170, 22);
	//this->_widget->add_radioButton(W_GUI_ID_RADIOBUTTON_3, 0, L"RadioButton 3", -300, 60, 170, 22);

	//wolf::gui::w_comboBox* _combo = nullptr;
	//this->_widget->add_comboBox(W_GUI_ID_COMBOBOX, 0, 80, 300, 22, -130, -12, 0, false, &_combo);
	//if (_combo)
	//{
	//	//_combo->set_drop_height(100);
	//	_combo->add_item(L"Combobox item (O)", (void*)0x11111111);
	//	_combo->add_item(L"Placeholder (O)", (void*)0x12121212);
	//	_combo->add_item(L"One more (O)", (void*)0x13131313);
	//	_combo->add_item(L"I can't get enough (O)", (void*)0x14141414);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"Ok, last one, I promise (O)", (void*)0x15151515);
	//	_combo->add_item(L"I can't get enough (O)", (void*)0x14141414);

	//}

	//_combo->set_comboBox_selected_color(W_COLOR(255, 0, 0, 255));

	//this->_widget->add_slider(W_GUI_ID_SLIDER_1, 50, 120, 250, 25, -100, 100);
	//this->_widget->add_slider(W_GUI_ID_SLIDER_2, 50, 150, 250, 25, -100, 100);
	//this->_widget->add_slider(W_GUI_ID_SLIDER_3, 50, 180, 250, 25, -100, 100);

}

static float step = 1;
void scene::update(const wolf::system::w_game_time& pGameTime)
{
	// TODO: Add your update logic here
	if (step == 10)
	{
		//		_list_widget->remove_item(1);
		//_widget->release();
		//		c->async_send("-r>10>11>Program-CG#EOF#\r\n");
	}
	step++;
	w_game::update(pGameTime);

}

void scene::begin_render(const wolf::system::w_game_time& pGameTime)
{

	w_game::begin_render(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	// TODO: Add your drawing code here
	wchar_t _preview_caprion[256];
	swprintf_s(_preview_caprion, L"Preview: %d fps", pGameTime.get_frames_per_second());
	//this->_widget->set_caption(_preview_caprion);

	auto _gDevice = this->graphics_devices.at(0);

	//	auto _v = camera->get_transform();
	//	logger.write(std::to_wstring(_v.x) + L" " + std::to_wstring(_v.y) + L" " + std::to_wstring(_v.z));

	if (sScene)
	{
		camera->update_view();
		camera->update_projection();
		//convert to XMMatrix
		auto _view_projection = DirectX::XMMatrixFromGLMMatrix(camera->get_view_projection());

		sScene->set_view_projection(_view_projection);
		sScene->render(pGameTime);
	}
	//float _dot_offset = -(fmodf(10 * pGameTime.get_total_seconds(), 2));

	//UNIQUE_RELEASE(this->_curve);
	//this->_curve.reset(new w_geometry(_gDevice));
	//this->_curve->set_color(w_color::from_hex(w_color::LIGHT_GRAY));
	//this->_curve->set_stroke_width(4);//3
	//this->_curve->set_fill_geometry(false);
	//this->_curve->set_style(StrokeStyleProperties(
	//	D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND,
	//	D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND,
	//	D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND,
	//	D2D1_LINE_JOIN::D2D1_LINE_JOIN_ROUND,
	//	10.0f,
	//	D2D1_DASH_STYLE::D2D1_DASH_STYLE_DOT,//D2D1_DASH_STYLE_SOLID
	//	_dot_offset));

	//std::array<D2D1_POINT_2F, 3> _array_points;
	//_array_points[0] = D2D1::Point2F(100, 100);
	//_array_points[1] = D2D1::Point2F(150, 50);
	//_array_points[2] = D2D1::Point2F(this->_global_mouse_point.x, this->_global_mouse_point.y);

	//std::vector<std::array<D2D1_POINT_2F, 3>> _points;
	//_points.push_back(_array_points);

	//this->_curve->begin();
	//{
	//	this->_curve->add_beziers(D2D1::Point2F(100, 100), _points,
	//		D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING,
	//		D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED,
	//		D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);
	//}
	//this->_curve->end();

	//this->sprite_batch->begin();
	//this->_curve->draw();
	//this->sprite_batch->end();

	for (size_t i = 0; i < _text_quads.size(); i++)
	{
		_text_quads[i]->set_view_projection(camera->get_view_projection());
		_text_quads[i]->set_position(DirectX::XMFLOAT3(1 - i * 0.64f, 0, 0));
		_text_quads[i]->render(_gDevice->context.Get());
	}

	//wolf::gui::w_gui::render(pGameTime);


	//this->sprite_batch->begin();


	//if (sBrush == nullptr)
	//{
	//	D2D1_COLOR_F _border_color;
	//	_border_color.r = 1.0;
	//	_border_color.g = 1.0;
	//	_border_color.b = 1.0;
	//	_border_color.a = 1.0;

	//	auto hr = _gDevice->context_2D->CreateSolidColorBrush(_border_color, &sBrush);
	//}

	//auto _m = sFont.get_font_metrics();
	//logger.write(std::to_string(_m.ascent));
	//logger.write(std::to_string(_m.capHeight));
	//logger.write(std::to_string(_m.descent));
	//logger.write(std::to_string(_m.designUnitsPerEm));
	//logger.write(std::to_string(_m.lineGap));
	//logger.write(std::to_string(_m.xHeight));


	//float _ratio = sFont.get_font_size()/ (float) _m.designUnitsPerEm;
	//float _size_rate = (_m.ascent + _m.descent + _m.lineGap) * _ratio;

	//int _retval = static_cast<int>(this->get_window_height() / _size_rate);
	//logger.write(std::to_string(_retval));
	//logger.write(std::to_string(_size_rate));

	//D2D1_RECT_F _rect;
	//sPathGeometry->GetBounds(D2D1::IdentityMatrix(), &_rect);

	//_gDevice->context_2D->DrawGeometry(sPathGeometry, sBrush);
	//_gDevice->context_2D->FillGeometry(sPathGeometry, sBrush);
	//
	////_rect.right += _size_rate;// 3 * sFont.get_font_size();
	////_rect.bottom += std::abs(_size_rate - _rect.bottom);
	////_rect.top += std::abs(_size_rate - _rect.top);

	//logger.write(std::to_string(std::abs(_rect.right - _rect.left)));
	//logger.write(std::to_string(std::abs(_rect.top - _rect.bottom)));

	//sprite_batch->draw_string(sText, _rect, sBrush, sDrawFont);

	//this->sprite_batch->end();

	w_game::render(pGameTime);
}

void scene::end_render(const wolf::system::w_game_time& pGameTime)
{

	w_game::end_render(pGameTime);
}

void scene::on_window_resized(UINT pIndex)
{
	// TODO: Add your logic for on window resizing event

	w_game::on_window_resized(pIndex);
}

void scene::on_device_lost()
{
	// TODO: Add your logic for on device lost event

	w_game::on_device_lost();
}

HRESULT scene::on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	if (this->_widget)
	{
		auto _procced = this->_widget->on_msg_proc(pHWND, pMessage, pWParam, pLParam);
		if (_procced)
		{
			return S_OK;
		}
	}

	switch (pMessage)
	{
	case WM_MOUSEMOVE:
		this->_global_mouse_point.x = short(LOWORD(pLParam));
		this->_global_mouse_point.y = short(HIWORD(pLParam));
		break;
	}

	//if not procced yet
	return S_FALSE;
}

static void CALLBACK on_gui_event(UINT pEvent, int pControlID, wolf::gui::w_control* pControl, void* pUserContext)
{
	switch (pControlID)
	{
	case W_GUI_ID_SLIDER_1:
	{
		auto _slider = (wolf::gui::w_slider*)pControl;
		auto _pos = camera->get_transform();
		camera->set_transform((float)_slider->get_value(), _pos.y, _pos.z);
		break;
	}
	case W_GUI_ID_SLIDER_2:
	{
		auto _slider = (wolf::gui::w_slider*)pControl;
		auto _pos = camera->get_transform();
		camera->set_transform(_pos.x, (float)_slider->get_value(), _pos.z);
		break;
	}
	case W_GUI_ID_SLIDER_3:
	{
		auto _slider = (wolf::gui::w_slider*)pControl;
		auto _pos = camera->get_transform();
		camera->set_transform(_pos.x, _pos.y, (float)_slider->get_value());
		break;
	}
	}
}

ULONG scene::release()
{
	if (this->get_is_released()) return 0;

	if (sScene)
	{
		sScene->release();
	}
	// TODO: Release your assets here
	//UNIQUE_RELEASE(this->spriteFont);
	//
	//UNIQUE_RELEASE(this->ellipse);
	//UNIQUE_RELEASE(this->rectangle);
	//	
	//UNIQUE_RELEASE(this->geoCloud0);
	//UNIQUE_RELEASE(this->geoCloud1);

	//UNIQUE_RELEASE(this->quad);

	//UNIQUE_RELEASE(this->renderTarget2D);
	//
	//UNIQUE_RELEASE(this->texture2D);

	//W_ffmpeg::ReleaseMF();

	wolf::gui::w_gui::release();

	return w_game::release();
}

