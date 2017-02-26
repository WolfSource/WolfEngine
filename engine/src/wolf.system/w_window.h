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

#ifdef __linux
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include "w_std.h"
#endif

#ifdef __APPLE__
#include "w_std.h"
#else
#include "w_system_export.h"
#include <memory>
#include "w_object.h"
#include "w_game_time.h"
#include <functional>
#include <vector>
#include <string>
#endif //__APPLE__

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

#ifdef __WIN32
struct w_enumerate_screens
{
	std::vector<RECT>   screens;
	RECT                combined;

	static BOOL CALLBACK w_enumerate_screens_callback(HMONITOR pHScreen, HDC pHDC, LPRECT pLRect, LPARAM pLParam)
	{
		auto _this = reinterpret_cast<w_enumerate_screens*>(pLParam);
		_this->screens.push_back(*pLRect);
		UnionRect(&_this->combined, &_this->combined, pLRect);
		return TRUE;
	}

	w_enumerate_screens()
	{
		SetRectEmpty(&this->combined);
		EnumDisplayMonitors(0, 0, w_enumerate_screens_callback, (LPARAM)this);
	}
};
#elif defined(__linux)

struct w_enumerate_screens
{
	std::vector<xcb_screen_t*>   screens;
        
	w_enumerate_screens()
	{
            //get the screen number(s)
            int _screen_num = -1;
            xcb_connection_t* _con = xcb_connect (NULL, &_screen_num);
           
            const xcb_setup_t* _setup = xcb_get_setup (_con);
  
            //get screens
            xcb_screen_iterator_t _iter = xcb_setup_roots_iterator(_setup);  

            //iterator all
            for (size_t i = 0; i <= _screen_num; ++i) 
            {
                screens.push_back(_iter.data);
                xcb_screen_next (&_iter);
            }
            
            xcb_disconnect(_con);
        }
};
#endif

//Store the information of window
struct w_window_info
{
#ifdef __WIN32
	HWND                hwnd;
	HINSTANCE           hInstance;
#elif defined(__linux)
    xcb_connection_t*   xcb_connection;
    xcb_window_t*       xcb_window;
#elif defined(__APPLE__)
    void*               window;
#endif
	UINT                width;
	UINT                height;
    bool                v_sync_enable;
};

#if defined(__WIN32) || defined(__linux)

class w_window : public wolf::system::w_object
{
public:
	WSYS_EXP w_window();
	WSYS_EXP virtual ~w_window();

#ifdef __WIN32
	//Initialize 
	WSYS_EXP HRESULT initialize(std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> pMsgProcFunction);
#elif defined(__linux)
        //Initialize 
	WSYS_EXP HRESULT initialize();
#endif
        //Run the main loop
	WSYS_EXP void run(std::function<void(void)>& pFunc);
	//Exit from main loop and close current window
	WSYS_EXP void close();

	//Release 
	WSYS_EXP virtual ULONG release() override;

#pragma region Setters

#ifdef __WIN32
	//Set class name of window 
	WSYS_EXP void set_class_name(_In_ LPWSTR pValue);
#endif
        
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
        
#elif defined(__linux)
    WSYS_EXP xcb_connection_t* get_xcb_connection() const
    {
        return this->_xcb_con;
    }
        
    WSYS_EXP xcb_window_t* get_xcb_window() const
    {
        return this->_xcb_window;
    }
#endif
        
	//Get absolute width of window
	WSYS_EXP UINT get_width() const;
	//Get absolute height of window
	WSYS_EXP UINT get_height() const;
	//Get fixed time step of game time
	WSYS_EXP bool get_fixed_time_step() const;
	//Get windows is full screen
	WSYS_EXP bool get_full_screen() const;

#pragma endregion

	//WSYS_EXP static std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> msg_proc_function;

private:
	typedef w_object                                _super;

	bool                                            _close;

#ifdef __WIN32
	std::wstring                                    _class_name;
	HINSTANCE                                       _hInstance;
	HWND                                            _hwnd;
    HDC                                             _hdc;
#elif __linux
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

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_WINDOW_H__
