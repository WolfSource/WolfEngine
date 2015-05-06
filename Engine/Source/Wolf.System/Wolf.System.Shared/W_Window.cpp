#include "Wolf.System.Shared.PCH.h"
#include "W_Window.h"

W_Window::W_Window() : fullScreen(false), screenWidth(800), screenHeight(600), screenPosX(0), screenPosY(0)
{
	SetFixedTimeStep(true);
}

W_Window::~W_Window()
{
	Release();
}

static LRESULT CALLBACK WndProc(HWND pHwnd, UINT pMessage, WPARAM pWparam, LPARAM pLparam)
{
	switch (pMessage)
	{
	default:
	{
		return DefWindowProc(pHwnd, pMessage, pWparam, pLparam);
	}
	//on destroying
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	// on closing
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	}
}

void W_Window::Initialize()
{
	appName = L"Wolf Engine";
	// Get the instance of this application.
	hInstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = appName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	this->screenWidth = GetSystemMetrics(SM_CXSCREEN);
	this->screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen
	DEVMODE devMode;

	if (this->fullScreen)
	{
		ZeroMemory(&devMode, sizeof(DEVMODE));
		devMode.dmSize = sizeof(devMode);
		devMode.dmPelsWidth = static_cast<unsigned long>(this->screenWidth);
		devMode.dmPelsHeight = static_cast<unsigned long>(this->screenHeight);
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Setup full screen mode
		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

	}
	else
	{
		this->screenWidth = 800;
		this->screenHeight = 600;

		//Middle screen
		this->screenPosX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		this->screenPosY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		appName,
		appName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		this->screenPosX,
		this->screenPosY,
		screenWidth,
		screenHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	// Show window up on the screen
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	// Hide/Show the mouse cursor.
	ShowCursor(true);

}

void W_Window::Run(std::function<void(void)>& const pFunc)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			break;
		}
		else
		{
			pFunc();
		}

		// Check if the user pressed escape and wants to quit.
		//if (m_Input->IsEscapePressed() == true) break;
	}
}

ULONG W_Window::Release()
{
	if (IsReleased()) return 0;
	
	this->appName = NULL;
	this->hInstance = NULL;
	this->hWnd = NULL;

	return W_Object::Release();
}

#pragma region Setters

void W_Window::SetFullScreen(bool pValue)
{
	this->fullScreen = pValue;
}

void W_Window::SetFixedTimeStep(bool pValue)
{
	this->gameTime.SetFixedTimeStep(pValue);
}

#pragma endregion

#pragma region Getters

HWND W_Window::GetHWND() const
{
	return this->hWnd;
}

int W_Window::GetWidth() const
{
	return this->screenWidth;
}

int W_Window::GetHeight() const
{
	return this->screenHeight;
}

bool W_Window::GetFixedTimeStep() const
{
	return this->gameTime.GetFixedTimeStep();
}

bool W_Window::GetFullScreen() const
{
	return this->fullScreen;
}

#pragma endregion