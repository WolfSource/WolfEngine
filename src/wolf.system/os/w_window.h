/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_window.h
    Description      : Responsible for creating window in OS
                       In linux, make sure run "sudo apt-get install libxcb1-dev"
                       In OSX, we do not need w_window class, all we need is w_present_info
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>
#include <memory/w_array.h>

#ifdef W_PLATFORM_ANDROID
#include <android/native_window.h>
#elif defined(W_PLATFORM_LINUX)
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

    //#include "python_exporter/w_boost_python_helper.h"

    typedef struct w_screen_coord_t
    {
        long left;
        long top;
        long right;
        long bottom;
    } w_screen_coord_t;
    typedef w_screen_coord_t* w_screen_coord;

    /**
     * enumerate screens
     * @param pMemPool The pool to allocate out of
     * @return arrays of w_screen_coord
     */
    W_SYSTEM_EXPORT
        w_array w_window_enumerate_screens(_Inout_ w_mem_pool pMemPool);


    //RECT                combined;
//#elif defined(W_PLATFORM_LINUX)
//
//    std::vector<xcb_screen_t*>   screens;
//    w_enumerate_screens()
//    {
//        //get the screen number(s)
//        int _screen_num = -1;
//        xcb_connection_t* _con = xcb_connect(NULL, &_screen_num);
//
//        const xcb_setup_t* _setup = xcb_get_setup(_con);
//
//        //get screens
//        xcb_screen_iterator_t _iter = xcb_setup_roots_iterator(_setup);
//
//        //iterator all
//        for (size_t i = 0; i <= _screen_num; ++i)
//        {
//            //TODO: convert to w_rectangle
//            screens.push_back(_iter.data);
//            xcb_screen_next(&_iter);
//        }
//
//        xcb_disconnect(_con);
//    }
//#endif
//
//#if defined(__PYTHON__)
//    boost::python::list py_screens()
//    {
//        boost::python::list _list;
//#ifndef W_PLATFORM_OSX
//        for (auto& _iter : screens)
//        {
//            _list.append(_iter);
//        }
//#endif
//        return _list;
//    }
//#endif



////store the information of presentation
//struct w_present_info
//{
//#if defined(W_PLATFORM_WIN) || defined(W_PLATFORM_LINUX) || defined(W_PLATFORM_OSX) || defined(W_PLATFORM_ANDROID)
//
//	uint32_t                width = 800;
//	uint32_t                height = 600;
//
//#if defined(W_PLATFORM_WIN) || defined(W_PLATFORM_LINUX) || defined(W_PLATFORM_OSX)
//	bool                v_sync = true;
//    bool				is_full_screen = false;
//#endif
//
//#ifdef W_PLATFORM_WIN
//	HWND                hwnd = NULL;
//	HINSTANCE           hInstance = NULL;
//#elif defined(W_PLATFORM_ANDROID)
//	ANativeWindow*		window = nullptr;
//#elif defined(W_PLATFORM_LINUX)
//	xcb_connection_t*   xcb_connection = nullptr;
//	xcb_window_t*       xcb_window = nullptr;
//#elif defined(W_PLATFORM_OSX)
//	void*               window = nullptr;
//#endif
//
//#elif defined(__UWP)
//	IUnknown*			window = nullptr;
//	Windows::Foundation::Rect	window_size;
//	Windows::Graphics::Display::DisplayOrientations	window_native_orientation;
//	Windows::Graphics::Display::DisplayOrientations	window_current_orientation;
//	float				window_dpi = 96;
//	bool				support_high_resolutions = true;
//#endif
//    
//    //BGRA8Unorm for DirectX12 is "87" and for VULKAN is 44
//	uint32_t			swap_chain_format = 44;
//    //set whether cpu can access to swap chain buffer, this flag does not have any effect when using embree as render platform
//    bool                cpu_access_swap_chain_buffer = false;
//    //set use double buffering, this flag does not have any effect when using embree as render platform
//	bool				double_buffering = true;
//
//    /*
//        following render platforms available:
//        vulkan : https://www.khronos.org/vulkan/
//        embree : https://embree.github.io/
//     */
//    std::string         platform;
//    /*
//     on target machine which has multiple graphics devices, you can
//     select preferred graphics device associated to this window, calling the function
//     w_graphics_device_manager::query_for_available_graphic_devices will help to get all graphics devices
//     */
//    size_t              preferred_graphics_device_index;
//    //title of window
//    std::string         title;
//    
//#ifdef __PYTHON__
//
//    void py_set_win_id(boost::python::object pWinID)
//    {
//#ifdef W_PLATFORM_WIN
//        boost::python::extract<unsigned int> _hwnd_int(pWinID);
//        if (_hwnd_int.check())
//        {
//            this->hwnd = (HWND)_hwnd_int();
//        }
//#elif defined(W_PLATFORM_OSX)
//        //return boost::python::object(window);
//#elif defined(W_PLATFORM_LINUX)
//        //return boost::python::object(xcb_window);
//#else
//        //return boost::python::object();
//#endif
//    }
//
//#endif
//
//};
//
//#if defined(W_PLATFORM_WIN) || (defined(W_PLATFORM_LINUX) && !defined(W_PLATFORM_ANDROID))
//
//class w_window
//{
//public:
//	WSYS_EXP w_window();
//	WSYS_EXP virtual ~w_window();
//
//#ifdef W_PLATFORM_WIN
//	//Initialize 
//	WSYS_EXP W_RESULT initialize(_In_ std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> pMsgProcFunction);
//#elif defined(W_PLATFORM_LINUX)
//        //Initialize 
//	WSYS_EXP W_RESULT initialize(_In_ bool pShow = true);
//#endif
//    //Run the main loop
//    template <class T>
//    void run(T&& pFunc)
//    {
//#ifdef W_PLATFORM_WIN
//        MSG msg;
//        std::memset(&msg, 0, sizeof(msg));
//
//        while (true)
//        {
//            auto _start = std::chrono::high_resolution_clock::now();
//            // Handle the windows messages.
//            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//            {
//                TranslateMessage(&msg);
//                DispatchMessage(&msg);
//                continue;
//            }
//            if (this->_close)
//            {
//                break;
//            }
//            pFunc();
//
//            auto _end = std::chrono::high_resolution_clock::now();
//            auto _delta = std::chrono::duration<double, std::milli>(_end - _start).count();
//            if (this->_id != -1)
//            {
//                wolf::windows_frame_time_in_sec.at((uint32_t)this->_id) = (float)_delta / 1000.0f;
//            }
//        }
//
//#elif defined(W_PLATFORM_LINUX) && !defined(W_PLATFORM_ANDROID)
//        
//        xcb_generic_event_t* _e = nullptr;
//        while (!this->_close)
//        {
//            while ((_e = xcb_poll_for_event(this->_xcb_con))) //xcb_wait_for_event
//            {
//                switch (_e->response_type & ~0x80)
//                {
//                case XCB_CLIENT_MESSAGE:
//                {
//                    if ((*(xcb_client_message_event_t*)_e).data.data32[0] == (*this->_atom_wm_delete_window).atom)
//                    {
//                        this->_close = true;
//                    }
//                    break;
//                }
//                case XCB_DESTROY_NOTIFY:
//                {
//                    this->_close = true;
//                    break;
//                }
//                case XCB_EXPOSE:
//                {
//                    auto _ev = (xcb_expose_event_t*)_e;
//                    //this->_window = _ev->window;
//                    this->_screen_posX = _ev->x;
//                    this->_screen_posY = _ev->y;
//                    this->_screen_width = _ev->width;
//                    this->_screen_height = _ev->height;
//
//                    break;
//                }
//                case XCB_BUTTON_PRESS:
//                {
//                    auto _ev = (xcb_button_press_event_t *)_e;
//                    switch (_ev->detail)
//                    {
//                    case 4:
//                    {
//                        //printf ("Wheel Button up in window %ld, at coordinates (%d,%d)\n",
//                        //ev->event, ev->event_x, ev->event_y);
//                        break;
//                    }
//                    case 5:
//                    {
//                        //printf ("Wheel Button down in window %ld, at coordinates (%d,%d)\n",
//                        //ev->event, ev->event_x, ev->event_y);
//                        break;
//                    }
//                    default:
//                    {
//                        //printf ("Button %d pressed in window %ld, at coordinates (%d,%d)\n",
//                        //ev->detail, ev->event, ev->event_x, ev->event_y);
//                    }
//                    }
//                    break;
//                }
//                case XCB_BUTTON_RELEASE:
//                {
//                    auto _ev = (xcb_button_release_event_t*)_e;
//                    //print_modifiers(ev->state);
//                    //printf ("Button %d released in window %ld, at coordinates (%d,%d)\n",
//                    //ev->detail, ev->event, ev->event_x, ev->event_y);
//                    break;
//                }
//                case XCB_MOTION_NOTIFY:
//                {
//                    auto _ev = (xcb_motion_notify_event_t*)_e;
//                    //printf ("Mouse moved in window %ld, at coordinates (%d,%d)\n",
//                    //ev->event, ev->event_x, ev->event_y);
//                    break;
//                }
//                case XCB_ENTER_NOTIFY:
//                {
//                    auto _ev = (xcb_enter_notify_event_t*)_e;
//                    //printf ("Mouse entered window %ld, at coordinates (%d,%d)\n",
//                    //ev->event, ev->event_x, ev->event_y);
//                    break;
//                }
//                case XCB_LEAVE_NOTIFY:
//                {
//                    auto _ev = (xcb_leave_notify_event_t*)_e;
//                    //printf ("Mouse left window %ld, at coordinates (%d,%d)\n",
//                    //ev->event, ev->event_x, ev->event_y);
//                    break;
//                }
//                case XCB_KEY_PRESS:
//                {
//                    auto _ev = (xcb_key_press_event_t *)_e;
//                    //print_modifiers(ev->state);
//                    //printf ("Key pressed %c in window %ld\n", ev->detail, ev->event);
//                    break;
//                }
//                case XCB_KEY_RELEASE:
//                {
//                    auto _ev = (xcb_key_release_event_t *)_e;
//                    //print_modifiers(ev->state);
//                    //printf ("Key released %d in window %ld\n", ev->detail, ev->event);
//                    break;
//                }
//                default:
//                {
//                    //Unknown event type, ignore it
//                    //printf("Unknown event: %d\n", e->response_type);
//                    break;
//                }
//                }
//                //free the generic event
//                free(_e);
//
//                if (this->_close) break;
//            }
//
//            pFunc();
//        }
//#endif
//    }
//
//	//Exit from main loop and close current window
//	WSYS_EXP void close();
//
//	//Release 
//	WSYS_EXP ULONG release();
//
//#pragma region Setters
//
//#ifdef W_PLATFORM_WIN
//	//Set class name of window 
//	WSYS_EXP void set_class_name(_In_ LPWSTR pValue);
//	//enable tiled
//	WSYS_EXP void enable_tiled(_In_ const bool& pValue);
//	//Set window caption
//	WSYS_EXP void set_caption(_In_ LPWSTR pValue);
//	//Set to enable border of window before creating window
//	WSYS_EXP void enable_border(_In_ const bool& pValue);
//	//Enable dialog dialog frame
//	WSYS_EXP void enable_dialog_frame(_In_ const bool& pValue);
//	//Set to enable system menu before creating window
//	WSYS_EXP void enable_system_menu(_In_ const bool& pValue);
//	//Set to enable caption
//	WSYS_EXP void enable_caption(_In_ const bool& pValue);
//#endif
//    
//    //Set ID of window
//    WSYS_EXP void set_id(_In_ const uint32_t& pValue);
//	//Set time step value of game time 
//	WSYS_EXP void set_fixed_time_step(bool pValue);
//	//Set windows is full screen
//	WSYS_EXP void set_full_screen(bool pValue);
//	//Set window with
//	WSYS_EXP void set_width(const int pValue);
//	//Set window height
//	WSYS_EXP void set_height(const int pValue);
//	//Set position of window 
//	WSYS_EXP void set_position(const int pX, const int pY);
//	//Set parent window 
//	WSYS_EXP void set_parent(
//#ifdef W_PLATFORM_WIN
//		_In_ HWND pHWND
//#endif
//	);
//
//#pragma endregion
//
//#pragma region Getters
//
//#ifdef W_PLATFORM_WIN
//	//Get calss name
//	WSYS_EXP LPCWSTR get_class_name() const;
//	//Get hwnd 
//	WSYS_EXP HWND get_HWND() const;
//	//Get hInstance
//	WSYS_EXP HINSTANCE get_HINSTANCE() const;
//	//Get hdc
//	WSYS_EXP HDC get_HDC() const;
//        
//#elif defined(W_PLATFORM_LINUX) && !defined(W_PLATFORM_ANDROID)
//    WSYS_EXP xcb_connection_t* get_xcb_connection() const
//    {
//        return this->_xcb_con;
//    }
//    WSYS_EXP xcb_window_t* get_xcb_window() const
//    {
//        return this->_xcb_window;
//    }
//#endif
//    
//    //Get ID of window
//    WSYS_EXP const uint32_t get_id() const;
//    //Get absolute width of window
//	WSYS_EXP uint32_t get_width() const;
//	//Get absolute height of window
//	WSYS_EXP uint32_t get_height() const;
//	//Get fixed time step of game time
//	WSYS_EXP bool get_fixed_time_step() const;
//	//Get windows is full screen
//	WSYS_EXP bool get_full_screen() const;
//
//#pragma endregion
//
//	//WSYS_EXP static std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> msg_proc_function;
//
//private:
//	bool											_is_released;
//	bool                                            _close;
//
//    int                                             _id;
//
//#ifdef W_PLATFORM_WIN
//	std::wstring                                    _class_name;
//	HINSTANCE                                       _hInstance;
//	HWND                                            _hwnd;
//    HDC                                             _hdc;
//	DWORD											_window_style;
//	HWND											_parent_hwnd;
//#elif defined(W_PLATFORM_LINUX) && !defined(W_PLATFORM_ANDROID)
//    xcb_connection_t*                               _xcb_con;
//    xcb_screen_t*                                   _xcb_screen;
//    xcb_window_t*                                   _xcb_window;
//    xcb_intern_atom_reply_t*                        _atom_wm_delete_window;
//#endif
//
//    std::wstring                                    _title;
//    std::wstring                                    _icon;
//	uint16_t                                        _screen_width;
//	uint16_t                                        _screen_height;
//	int16_t                                         _screen_posX;
//	int16_t                                         _screen_posY;
//	bool                                            _full_screen;
//	bool                                            _fixed_time_step;
//	w_gametime                                      _game_time;
//};
//
//#endif //__WIN32 && __linux

//#include "python_exporter/py_window.h"

#ifdef __cplusplus
}
#endif