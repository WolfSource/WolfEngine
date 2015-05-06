/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_Window.h
	Description		 : The window
	Comment          :
*/

#pragma once

#include <memory>
#include "W_Object.h"
#include "W_GameTime.h"
#include <functional>

//Store the information of window
struct W_WindowInfo
{
	HWND hWnd;
	UINT width;
	UINT height;
};

class W_Window : public Wolf::System::W_Object
{
public:
	API W_Window();
	API virtual ~W_Window();

	API void Initialize();
	API void Run(std::function<void(void)>& const pFunc);

	API virtual ULONG Release() override;

#pragma region Setters

	API void SetFixedTimeStep(bool pValue);
	API void SetFullScreen(bool pValue);

#pragma endregion

#pragma region Getters

	API HWND GetHWND() const;
	API int GetWidth() const;
	API int GetHeight() const;
	API bool GetFixedTimeStep() const;
	API bool GetFullScreen() const;

#pragma endregion

private:
	LPCWSTR appName;
	HINSTANCE hInstance;
	HWND hWnd;
	int screenWidth;
	int screenHeight;
	int screenPosX, screenPosY;
	bool fullScreen;
	bool fixedTimeStep;
	Wolf::System::W_GameTime gameTime;
};

