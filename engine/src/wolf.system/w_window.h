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

#include <memory>
#include "w_object.h"
#include "w_system_dll.h"
#include "w_game_time.h"
#include <functional>
#include <vector>

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
	HWND hWnd;
	UINT width;
	UINT height;
};

class w_window : public wolf::system::w_object
{
public:
	SYS_EXP w_window();
	SYS_EXP virtual ~w_window();

	//Initialize 
	SYS_EXP void initialize();
	//Run the main loop
	SYS_EXP void run(std::function<void(void)>& const pFunc);

	//Exit from main loop and close current window
	SYS_EXP void close();

	//Release 
	SYS_EXP virtual ULONG release() override;

#pragma region Setters

	//Set time step value of game time 
	SYS_EXP void set_fixed_timeStep(bool pValue);
	//Set windows is fullscreen
	SYS_EXP void set_fullScreen(bool pValue);
	//Set window with
	SYS_EXP void set_width(const int pValue);
	//Set window height
	SYS_EXP void set_height(const int pValue);
	//Set position of window 
	SYS_EXP void set_position(const int pX, const int pY);

#pragma endregion

#pragma region Getters

	//Get hwnd 
	SYS_EXP HWND get_HWND() const;
	//Get hdc
	SYS_EXP HDC get_HDC() const;
	//Get absolute width of window
	SYS_EXP UINT get_width() const;
	//Get absolute height of window
	SYS_EXP UINT get_height() const;
	//Get fixed time step of game time
	SYS_EXP bool get_fixed_timeStep() const;
	//Get windows is fullscreen
	SYS_EXP bool get_fullScreen() const;

#pragma endregion

	SYS_EXP static std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> msg_proc_func;

private:
	typedef w_object _super;

	bool						_close;
	LPCWSTR						_app_name;
	HINSTANCE					_hInstance;
	HWND						_hWnd;
	HDC							_hdc;
	int							_screen_width;
	int							_screen_height;
	int							_screen_posX;
	int							_screen_posY;
	bool						_full_screen;
	bool						_fixed_time_step;
	wolf::system::w_game_time	_game_time;
};

#endif