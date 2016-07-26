#include "w_system_pch.h"
#include "w_window.h"

std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> w_window::msg_proc_func = nullptr;

w_window::w_window() :
	_fullScreen(false), 
	_screenWidth(800), 
	_screenHeight(600), 
	_screenPosX(0), 
	_screenPosY(0),
	_close(false)
{
	set_fixed_timeStep(true);
}

w_window::~w_window()
{
	release();
}

static LRESULT CALLBACK MsgProc(HWND pHwnd, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	if (w_window::msg_proc_func != nullptr && w_window::msg_proc_func(pHwnd, pMessage, pWParam, pLParam) == S_OK)
	{
		return S_OK;
	}
	return DefWindowProc(pHwnd, pMessage, pWParam, pLParam);
}

void w_window::initialize()
{
	//Unregister all
	this->_hInstance = NULL;
	this->_hWnd = NULL;
	this->_hdc = NULL;
	this->_appName = L"Wolf Engine";
	UnregisterClass(this->_appName, NULL);


	// Get the instance of this application.
	this->_hInstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = MsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->_appName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	this->_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	this->_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen
	DEVMODE devMode;

	if (this->_fullScreen)
	{
		ZeroMemory(&devMode, sizeof(DEVMODE));
		devMode.dmSize = sizeof(devMode);
		devMode.dmPelsWidth = static_cast<unsigned long>(this->_screenWidth);
		devMode.dmPelsHeight = static_cast<unsigned long>(this->_screenHeight);
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Setup full screen mode
		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

	}
	else
	{
		this->_screenWidth = 800;
		this->_screenHeight = 600;

		//Middle screen
		this->_screenPosX = (GetSystemMetrics(SM_CXSCREEN) - this->_screenWidth) / 2;
		this->_screenPosY = (GetSystemMetrics(SM_CYSCREEN) - this->_screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	this->_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		this->_appName,
		this->_appName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		this->_screenPosX,
		this->_screenPosY,
		this->_screenWidth,
		this->_screenHeight,
		NULL,
		NULL,
		this->_hInstance,
		NULL);
	if (!this->_hWnd)
	{
		V(S_FALSE, L"creating window handle", this->name, 3, true, false);
		return;
	}

	this->_hdc = GetDC(this->_hWnd);

	// Show window up on the screen
	ShowWindow(this->_hWnd, SW_SHOW);
	SetForegroundWindow(this->_hWnd);
	SetFocus(this->_hWnd);

	// Hide/Show the mouse cursor.
	ShowCursor(true);

}

void w_window::run(std::function<void(void)>& const pFunc)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		if (this->_close) break;
		
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			pFunc();
		}
	}
}

void w_window::close()
{
	this->_close = true;
}

ULONG w_window::release()
{
	if (is_released()) return 0;
	
	this->_hInstance = NULL;
	this->_hWnd = NULL;
	this->_hdc = NULL;

	UnregisterClass(this->_appName, NULL);
	this->_appName = NULL;

	return w_object::release();
}

#pragma region Setters

void w_window::set_fullScreen(bool pValue)
{
	this->_fullScreen = pValue;
}

void w_window::set_fixed_timeStep(bool pValue)
{
	this->_gameTime.set_fixed_time_step(pValue);
}

#pragma endregion

#pragma region Getters

HDC w_window::get_HDC() const
{
	return this->_hdc;
}

HWND w_window::get_HWND() const
{
	return this->_hWnd;
}

UINT w_window::get_width() const
{
	return this->_screenWidth;
}

UINT w_window::get_height() const
{
	return this->_screenHeight;
}

bool w_window::get_fixed_timeStep() const
{
	return this->_gameTime.get_fixed_timeStep();
}

bool w_window::get_fullScreen() const
{
	return this->_fullScreen;
}

#pragma endregion