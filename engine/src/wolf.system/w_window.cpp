#include "w_system_pch.h"
#include "w_window.h"

std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> w_window::msg_proc_func = nullptr;

w_window::w_window() :
	_full_screen(false), 
	_screen_width(800), 
	_screen_height(600), 
	_screen_posX(-1), 
	_screen_posY(-1),
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
	this->_app_name = L"Wolf Engine";
	UnregisterClass(this->_app_name, NULL);


	// Get the instance of this application.
	this->_hInstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; //CS_DBLCLKS for double click
	wc.lpfnWndProc = MsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->_app_name;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Setup the screen
	DEVMODE devMode;

	if (this->_full_screen)
	{
		// Determine the resolution of the clients desktop screen and create full screen window

		ZeroMemory(&devMode, sizeof(DEVMODE));
		devMode.dmSize = sizeof(devMode);
		devMode.dmPelsWidth = static_cast<unsigned long>(GetSystemMetrics(SM_CXSCREEN));
		devMode.dmPelsHeight = static_cast<unsigned long>(GetSystemMetrics(SM_CYSCREEN));
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Setup full screen mode
		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

	}
	else
	{
		//Middle screen
		if (this->_screen_posX == -1 || this->_screen_posY == -1)
		{
			this->_screen_posX = (GetSystemMetrics(SM_CXSCREEN) - this->_screen_width) / 2;
			this->_screen_posY = (GetSystemMetrics(SM_CYSCREEN) - this->_screen_height) / 2;
		}
	}

	// Create the window with the screen settings and get the handle to it.
	this->_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		this->_app_name,
		this->_app_name,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		this->_screen_posX,
		this->_screen_posY,
		this->_screen_width,
		this->_screen_height,
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

	UnregisterClass(this->_app_name, NULL);
	this->_app_name = NULL;

	return w_object::release();
}

#pragma region Setters

void w_window::set_fullScreen(bool pValue)
{
	this->_full_screen = pValue;
}

void w_window::set_fixed_timeStep(bool pValue)
{
	this->_game_time.set_fixed_time_step(pValue);
}

void w_window::set_width(const int pValue)
{
	this->_screen_width = pValue;
}

void w_window::set_height(const int pValue)
{
	this->_screen_height = pValue;
}

void w_window::set_position(const int pX, const int pY)
{
	this->_screen_posX = pX;
	this->_screen_posY = pY;
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
	return this->_screen_width;
}

UINT w_window::get_height() const
{
	return this->_screen_height;
}

bool w_window::get_fixed_timeStep() const
{
	return this->_game_time.get_fixed_timeStep();
}

bool w_window::get_fullScreen() const
{
	return this->_full_screen;
}

#pragma endregion