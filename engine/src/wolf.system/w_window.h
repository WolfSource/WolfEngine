/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_window.h
	Description		 : The window
	Comment          :
*/

#ifndef __W_WINDOW_H__
#define __W_WINDOW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "w_system_export.h"
#include <memory>
#include "w_object.h"
#include "w_game_time.h"
#include <functional>
#include <vector>
#include <string>

struct w_enumerate_monitors
{
	std::vector<RECT>   monitors;
	RECT                combined;

	static BOOL CALLBACK enumerate_monitors(HMONITOR pHMonitor, HDC pHDC, LPRECT pLRect, LPARAM pLParam)
	{
		auto _this = reinterpret_cast<w_enumerate_monitors*>(pLParam);
		_this->monitors.push_back(*pLRect);
		UnionRect(&_this->combined, &_this->combined, pLRect);
		return TRUE;
	}

	w_enumerate_monitors()
	{
		SetRectEmpty(&this->combined);
		EnumDisplayMonitors(0, 0, enumerate_monitors, (LPARAM)this);
	}
};

//Store the information of window
struct w_window_info
{
	HWND		hwnd;
	HINSTANCE	hInstance;
	UINT		width;
	UINT		height;
};

class w_window : public wolf::system::w_object
{
public:
	WSYS_EXP w_window();
	WSYS_EXP virtual ~w_window();

	//Initialize 
	WSYS_EXP void initialize(std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> pMsgProcFunction);
	//Run the main loop
	WSYS_EXP void run(std::function<void(void)>& const pFunc);

	//Exit from main loop and close current window
	WSYS_EXP void close();

	//Release 
	WSYS_EXP virtual ULONG release() override;

#pragma region Setters

	//Set class name of window 
	WSYS_EXP void set_class_name(_In_ LPWSTR pValue);
	//Set time step value of game time 
	WSYS_EXP void set_fixed_time_step(bool pValue);
	//Set windows is fullscreen
	WSYS_EXP void set_full_screen(bool pValue);
	//Set window with
	WSYS_EXP void set_width(const int pValue);
	//Set window height
	WSYS_EXP void set_height(const int pValue);
	//Set position of window 
	WSYS_EXP void set_position(const int pX, const int pY);

#pragma endregion

#pragma region Getters

	//Get calss name
	WSYS_EXP LPCWSTR get_class_name() const;
	//Get hwnd 
	WSYS_EXP HWND get_HWND() const;
	//Get hInstance
	WSYS_EXP HINSTANCE get_HINSTANCE() const;
	//Get hdc
	WSYS_EXP HDC get_HDC() const;
	//Get absolute width of window
	WSYS_EXP UINT get_width() const;
	//Get absolute height of window
	WSYS_EXP UINT get_height() const;
	//Get fixed time step of game time
	WSYS_EXP bool get_fixed_time_step() const;
	//Get windows is fullscreen
	WSYS_EXP bool get_full_screen() const;

#pragma endregion

	//WSYS_EXP static std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> msg_proc_function;

private:
	typedef w_object _super;

	bool						_close;
	std::wstring				_class_name;
	HINSTANCE					_hInstance;
	HWND						_hwnd;
	HDC							_hdc;
	int							_screen_width;
	int							_screen_height;
	int							_screen_posX;
	int							_screen_posY;
	bool						_full_screen;
	bool						_fixed_time_step;
	wolf::system::w_game_time	_game_time;
};

#endif //__W_WINDOW_H__