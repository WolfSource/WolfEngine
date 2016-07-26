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

//Store the information of window
struct W_WindowInfo
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
	LPCWSTR						_appName;
	HINSTANCE					_hInstance;
	HWND						_hWnd;
	HDC							_hdc;
	int							_screenWidth;
	int							_screenHeight;
	int							_screenPosX;
	int							_screenPosY;
	bool						_fullScreen;
	bool						_fixedTimeStep;
	wolf::system::w_game_time	_gameTime;
};

#endif