/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_window.h
	Description		 : The class responsible for creating window in OS
                                   In linux, make sure run "sudo apt-get install libxcb1-dev"
                                   In OSX, we do not need w_window class, all we need is w_window_info
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_WINDOW_H__
#define __W_WINDOW_H__

#ifdef __ANDROID
#include <android/native_window.h>
#elif defined(__linux)
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

#if defined(__APPLE__) || defined(__linux) || defined(__ANDROID)
#include "w_std.h"
#endif

#include "w_system_export.h"
#include <memory>
#include "w_object.h"
#include "w_game_time.h"
#include "w_rectangle.h"
#include <functional>
#include <vector>
#include <string>
#include <chrono>

#ifdef __UWP
#include <wrl.h>
#include <wrl/client.h>
#include <agile.h>
#endif

#include "python_exporter/w_boost_python_helper.h"

struct w_enumerate_screens
{
#ifdef __WIN32
	std::vector<w_rectangle>   screens;
	//RECT                combined;

	static BOOL CALLBACK w_enumerate_screens_callback(HMONITOR pHScreen, HDC pHDC, LPRECT pLRect, LPARAM pLParam)
	{
		auto _this = reinterpret_cast<w_enumerate_screens*>(pLParam);
        if (pLRect)
        {
            w_rectangle _rc;
            _rc.left = pLRect->left;
            _rc.right = pLRect->right;
            _rc.top = pLRect->top;
            _rc.bottom = pLRect->bottom;
            _this->screens.push_back(_rc);
        }
		//UnionRect(&_this->combined, &_this->combined, pLRect);
		return TRUE;
	}

	w_enumerate_screens()
	{
		//SetRectEmpty(&this->combined);
		EnumDisplayMonitors(0, 0, w_enumerate_screens_callback, (LPARAM)this);
	}

#elif defined(__linux)

    std::vector<xcb_screen_t*>   screens;
    w_enumerate_screens()
    {
        //get the screen number(s)
        int _screen_num = -1;
        xcb_connection_t* _con = xcb_connect(NULL, &_screen_num);

        const xcb_setup_t* _setup = xcb_get_setup(_con);

        //get screens
        xcb_screen_iterator_t _iter = xcb_setup_roots_iterator(_setup);

        //iterator all
        for (size_t i = 0; i <= _screen_num; ++i)
        {
            //TODO: convert to w_rectangle
            screens.push_back(_iter.data);
            xcb_screen_next(&_iter);
        }

        xcb_disconnect(_con);
    }
#endif

#if defined(__PYTHON__)
    boost::python::list py_screens()
    {
        boost::python::list _list;
#ifndef __APPLE__
        for (auto& _iter : screens)
        {
            _list.append(_iter);
        }
#endif
        return _list;
    }
#endif

};

//store the information of presentation
struct w_present_info
{
#if defined(__WIN32) || defined(__linux) || defined(__APPLE__) || defined(__ANDROID)

	uint32_t                width = 800;
	uint32_t                height = 600;

#if defined(__WIN32) || defined(__linux) || defined(__APPLE__)
	bool                v_sync = true;
	bool			    force_to_disable_v_sync = false;
    bool				is_full_screen = false;
#endif

#ifdef __WIN32
	HWND                hwnd = NULL;
	HINSTANCE           hInstance = NULL;
#elif defined(__ANDROID)
	ANativeWindow*		window = nullptr;
#elif defined(__linux)
	xcb_connection_t*   xcb_connection = nullptr;
	xcb_window_t*       xcb_window = nullptr;
#elif defined(__APPLE__)
	void*               window = nullptr;
#endif

#elif defined(__UWP)
	IUnknown*			window = nullptr;
	Windows::Foundation::Rect	window_size;
	Windows::Graphics::Display::DisplayOrientations	window_native_orientation;
	Windows::Graphics::Display::DisplayOrientations	window_current_orientation;
	float				window_dpi = 96;
	bool				support_high_resolutions = true;
#endif
    
    //BGRA8Unorm for DirectX12 is "87" and for VULKAN is 44
	uint32_t			swap_chain_format = 44;
    //set whether cpu can access to swap chain buffer, this flag does not have any effect when using embree as render platform
    bool                cpu_access_swap_chain_buffer = false;
    //set use double buffering, this flag does not have any effect when using embree as render platform
	bool				double_buffering = true;

    /*
        following render platforms available:
        vulkan : https://www.khronos.org/vulkan/
        embree : https://embree.github.io/
     */
    std::string         platform;
    /*
     on target machine which has multiple graphics devices, you can
     select preferred graphics device associated to this window, calling the function
     w_graphics_device_manager::query_for_available_graphic_devices will help to get all graphics devices
     */
    size_t              preferred_graphics_device_index;
    //title of window
    std::string         title;
    
#ifdef __PYTHON__

    void py_set_win_id(boost::python::object pWinID)
    {
#ifdef __WIN32
        boost::python::extract<unsigned int> _hwnd_int(pWinID);
        if (_hwnd_int.check())
        {
            this->hwnd = (HWND)_hwnd_int();
        }
#elif defined(__APPLE__)
        //return boost::python::object(window);
#elif defined(__linux)
        //return boost::python::object(xcb_window);
#else
        //return boost::python::object();
#endif
    }

#endif

};

#if defined(__WIN32) || (defined(__linux) && !defined(__ANDROID))

class w_window : public wolf::system::w_object
{
public:
	WSYS_EXP w_window();
	WSYS_EXP virtual ~w_window();

#ifdef __WIN32
	//Initialize 
	WSYS_EXP W_RESULT initialize(std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> pMsgProcFunction);
#elif defined(__linux)
        //Initialize 
	WSYS_EXP W_RESULT initialize();
#endif
    //Run the main loop
    template <class T>
    void run(T&& pFunc)
    {
#ifdef __WIN32
        MSG msg;
        std::memset(&msg, 0, sizeof(msg));

        while (true)
        {
            auto _start = std::chrono::high_resolution_clock::now();
            // Handle the windows messages.
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                continue;
            }
            if (this->_close)
            {
                break;
            }
            pFunc();

            auto _end = std::chrono::high_resolution_clock::now();
            auto _delta = std::chrono::duration<double, std::milli>(_end - _start).count();
            if (this->_id != -1)
            {
                wolf::windows_frame_time_in_sec.at((uint32_t)this->_id) = (float)_delta / 1000.0f;
            }
        }

#elif defined(__linux) && !defined(__ANDROID)
        
        xcb_generic_event_t* _e = nullptr;
        while (!this->_close)
        {
            while (_e = xcb_poll_for_event(this->_xcb_con)) //xcb_wait_for_event
            {
                switch (_e->response_type & ~0x80)
                {
                case XCB_CLIENT_MESSAGE:
                {
                    if ((*(xcb_client_message_event_t*)_e).data.data32[0] == (*this->_atom_wm_delete_window).atom)
                    {
                        this->_close = true;
                    }
                    break;
                }
                case XCB_DESTROY_NOTIFY:
                {
                    this->_close = true;
                    break;
                }
                case XCB_EXPOSE:
                {
                    auto _ev = (xcb_expose_event_t*)_e;
                    //this->_window = _ev->window;
                    this->_screen_posX = _ev->x;
                    this->_screen_posY = _ev->y;
                    this->_screen_width = _ev->width;
                    this->_screen_height = _ev->height;

                    break;
                }
                case XCB_BUTTON_PRESS:
                {
                    auto _ev = (xcb_button_press_event_t *)_e;
                    switch (_ev->detail)
                    {
                    case 4:
                    {
                        //printf ("Wheel Button up in window %ld, at coordinates (%d,%d)\n",
                        //ev->event, ev->event_x, ev->event_y);
                        break;
                    }
                    case 5:
                    {
                        //printf ("Wheel Button down in window %ld, at coordinates (%d,%d)\n",
                        //ev->event, ev->event_x, ev->event_y);
                        break;
                    }
                    default:
                    {
                        //printf ("Button %d pressed in window %ld, at coordinates (%d,%d)\n",
                        //ev->detail, ev->event, ev->event_x, ev->event_y);
                    }
                    }
                    break;
                }
                case XCB_BUTTON_RELEASE:
                {
                    auto _ev = (xcb_button_release_event_t*)_e;
                    //print_modifiers(ev->state);
                    //printf ("Button %d released in window %ld, at coordinates (%d,%d)\n",
                    //ev->detail, ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_MOTION_NOTIFY:
                {
                    auto _ev = (xcb_motion_notify_event_t*)_e;
                    //printf ("Mouse moved in window %ld, at coordinates (%d,%d)\n",
                    //ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_ENTER_NOTIFY:
                {
                    auto _ev = (xcb_enter_notify_event_t*)_e;
                    //printf ("Mouse entered window %ld, at coordinates (%d,%d)\n",
                    //ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_LEAVE_NOTIFY:
                {
                    auto _ev = (xcb_leave_notify_event_t*)_e;
                    //printf ("Mouse left window %ld, at coordinates (%d,%d)\n",
                    //ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_KEY_PRESS:
                {
                    auto _ev = (xcb_key_press_event_t *)_e;
                    //print_modifiers(ev->state);
                    //printf ("Key pressed %c in window %ld\n", ev->detail, ev->event);
                    break;
                }
                case XCB_KEY_RELEASE:
                {
                    auto _ev = (xcb_key_release_event_t *)_e;
                    //print_modifiers(ev->state);
                    //printf ("Key released %d in window %ld\n", ev->detail, ev->event);
                    break;
                }
                default:
                {
                    //Unknown event type, ignore it
                    //printf("Unknown event: %d\n", e->response_type);
                    break;
                }
                }
                //free the generic event
                free(_e);

                if (this->_close) break;
            }

            pFunc();
        }
#endif
    }

	//Exit from main loop and close current window
	WSYS_EXP void close();

	//Release 
	WSYS_EXP virtual ULONG release() override;

#pragma region Setters

#ifdef __WIN32
	//Set class name of window 
	WSYS_EXP void set_class_name(_In_ LPWSTR pValue);
	//enable tiled
	WSYS_EXP void enable_tiled(_In_ const bool& pValue);
	//Set window caption
	WSYS_EXP void set_caption(_In_ LPWSTR pValue);
	//Set to enable border of window before creating window
	WSYS_EXP void enable_border(_In_ const bool& pValue);
	//Enable dialog dialog frame
	WSYS_EXP void enable_dialog_frame(_In_ const bool& pValue);
	//Set to enable system menu before creating window
	WSYS_EXP void enable_system_menu(_In_ const bool& pValue);
	//Set to enable caption
	WSYS_EXP void enable_caption(_In_ const bool& pValue);
#endif
    
    //Set ID of window
    WSYS_EXP void set_id(_In_ const uint32_t& pValue);
	//Set time step value of game time 
	WSYS_EXP void set_fixed_time_step(bool pValue);
	//Set windows is full screen
	WSYS_EXP void set_full_screen(bool pValue);
	//Set window with
	WSYS_EXP void set_width(const int pValue);
	//Set window height
	WSYS_EXP void set_height(const int pValue);
	//Set position of window 
	WSYS_EXP void set_position(const int pX, const int pY);

#pragma endregion

#pragma region Getters

#ifdef __WIN32
	//Get calss name
	WSYS_EXP LPCWSTR get_class_name() const;
	//Get hwnd 
	WSYS_EXP HWND get_HWND() const;
	//Get hInstance
	WSYS_EXP HINSTANCE get_HINSTANCE() const;
	//Get hdc
	WSYS_EXP HDC get_HDC() const;
        
#elif defined(__linux) && !defined(__ANDROID)
    WSYS_EXP xcb_connection_t* get_xcb_connection() const
    {
        return this->_xcb_con;
    }
    WSYS_EXP xcb_window_t* get_xcb_window() const
    {
        return this->_xcb_window;
    }
#endif
    
    //Get ID of window
    WSYS_EXP const uint32_t get_id() const;
    //Get absolute width of window
	WSYS_EXP uint32_t get_width() const;
	//Get absolute height of window
	WSYS_EXP uint32_t get_height() const;
	//Get fixed time step of game time
	WSYS_EXP bool get_fixed_time_step() const;
	//Get windows is full screen
	WSYS_EXP bool get_full_screen() const;

#pragma endregion

	//WSYS_EXP static std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> msg_proc_function;

private:
	typedef w_object                                _super;

	bool                                            _close;

    int                                             _id;

#ifdef __WIN32
	std::wstring                                    _class_name;
	HINSTANCE                                       _hInstance;
	HWND                                            _hwnd;
    HDC                                             _hdc;
	DWORD											_window_style;
#elif defined(__linux) && !defined(__ANDROID)
    xcb_connection_t*                               _xcb_con;
    xcb_screen_t*                                   _xcb_screen;
    xcb_window_t*                                   _xcb_window;
    xcb_intern_atom_reply_t*                        _atom_wm_delete_window;
#endif

    std::wstring                                    _title;
    std::wstring                                    _icon;
	uint16_t                                        _screen_width;
	uint16_t                                        _screen_height;
	int16_t                                         _screen_posX;
	int16_t                                         _screen_posY;
	bool                                            _full_screen;
	bool                                            _fixed_time_step;
	wolf::system::w_game_time                       _game_time;
};

#endif //__WIN32 && __linux

#include "python_exporter/py_window.h"

#endif //__W_WINDOW_H__
