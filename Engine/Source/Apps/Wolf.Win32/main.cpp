#include "Wolf.Win32.PCH.h"
#include "Scene.h"

using namespace std;

#ifndef FULLSCREEN
//#define FULLSCREEN
#endif

static LPCWSTR appName;
static HINSTANCE hInstance;
static HWND hWnd;
static std::unique_ptr<Scene> scene;
static map<int, vector<W_WindowInfo>> windowsInfo;

//Forward declration
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static void Initialize()
{	
	int screenWidth, screenHeight, posX, posY;

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
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen
	DEVMODE devMode;

#ifdef FULLSCREEN

	ZeroMemory(&devMode, sizeof(DEVMODE));
	devMode.dmSize = sizeof(devMode);
	devMode.dmPelsWidth = static_cast<unsigned long>(screenWidth);
	devMode.dmPelsHeight = static_cast<unsigned long>(screenHeight);
	devMode.dmBitsPerPel = 32;
	devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//Left up
	posX = 0;
	posY = 0;

	//Setup full screen mode
	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

#else

	screenWidth = 800;
	screenHeight = 600;

	//Middle screen
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

#endif

	// Create the window with the screen settings and get the handle to it.
	hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		appName,
		appName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX,
		posY,
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

	//Create how many windows do you want as an output window of this game
	using namespace Wolf::Graphics;
	vector<W_WindowInfo> win = { { hWnd, screenWidth, screenHeight } };
	windowsInfo[0] = win;

	scene = make_unique<Scene>();
}

static void Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	Wolf::System::W_GameTime gameTime;
	gameTime.SetFixedTimeStep(true);

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
			scene->Run(windowsInfo);
		}

		// Check if the user pressed escape and wants to quit.
		//if (m_Input->IsEscapePressed() == true) break;
	}
}

static void Release()
{
	SMART_RELEASE(scene);
}

LRESULT CALLBACK WndProc(HWND pHwnd, UINT pMessage, WPARAM pWparam, LPARAM pLparam)
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

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	Initialize();
	Run();
	Release();

	return EXIT_SUCCESS;
}