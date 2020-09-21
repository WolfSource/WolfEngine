/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_gui_state.h
	Description		 : This class responsible to storeing gui state and data access is done with thread safety (if g_bThreadSafe==true). This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_GUI_STATE__
#define __W_GUI_STATE__

class w_gui_state
{
protected:
	struct STATE
	{
		DXUTDeviceSettings*     m_CurrentDeviceSettings;   // current device settings
		IDXGIFactory1*          m_DXGIFactory;             // DXGI Factory object
		IDXGIAdapter1*          m_DXGIAdapter;            // The DXGI adapter object for the D3D11 device
		IDXGIOutput**           m_DXGIOutputArray;        // The array of output obj for the D3D11 adapter obj
		UINT                    m_DXGIOutputArraySize;    // Number of elements in m_D3D11OutputArray
		IDXGISwapChain*         m_DXGISwapChain;          // the D3D11 swapchain
		DXGI_SURFACE_DESC       m_BackBufferSurfaceDescDXGI; // D3D11 back buffer surface description
		bool                    m_RenderingOccluded;       // Rendering is occluded by another window
		bool                    m_DoNotStoreBufferSize;    // Do not store the buffer size on WM_SIZE messages

														   // D3D11 specific
		ID3D11Device*           m_D3D11Device;             // the D3D11 rendering device
		ID3D11DeviceContext*	m_D3D11DeviceContext;	   // the D3D11 immediate device context
		D3D_FEATURE_LEVEL       m_D3D11FeatureLevel;	   // the D3D11 feature level that this device supports
		ID3D11Texture2D*        m_D3D11DepthStencil;       // the D3D11 depth stencil texture (optional)
		ID3D11DepthStencilView* m_D3D11DepthStencilView;   // the D3D11 depth stencil view (optional)
		ID3D11RenderTargetView* m_D3D11RenderTargetView;   // the D3D11 render target view
		ID3D11RasterizerState*  m_D3D11RasterizerState;    // the D3D11 Rasterizer state

														   // D3D11.1 specific
		ID3D11Device1*          m_D3D11Device1;            // the D3D11.1 rendering device
		ID3D11DeviceContext1*	m_D3D11DeviceContext1;	   // the D3D11.1 immediate device context

#ifdef USE_DIRECT3D11_2
														   // D3D11.2 specific
		ID3D11Device2*          m_D3D11Device2;            // the D3D11.2 rendering device
		ID3D11DeviceContext2*	m_D3D11DeviceContext2;	   // the D3D11.2 immediate device context
#endif

														   // General
		HWND  m_HWNDFocus;                  // the main app focus window
		HWND  m_HWNDDeviceFullScreen;       // the main app device window in fullscreen mode
		HWND  m_HWNDDeviceWindowed;         // the main app device window in windowed mode
		HMONITOR m_AdapterMonitor;          // the monitor of the adapter 
		HMENU m_Menu;                       // handle to menu

		UINT m_FullScreenBackBufferWidthAtModeChange;  // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_FullScreenBackBufferHeightAtModeChange; // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_WindowBackBufferWidthAtModeChange;  // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		UINT m_WindowBackBufferHeightAtModeChange; // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		DWORD m_WindowedStyleAtModeChange;  // window style
		WINDOWPLACEMENT m_WindowedPlacement;// record of windowed HWND position/show state/etc
		bool  m_TopmostWhileWindowed;       // if true, the windowed HWND is topmost 
		bool  m_Minimized;                  // if true, the HWND is minimized
		bool  m_Maximized;                  // if true, the HWND is maximized
		bool  m_MinimizedWhileFullscreen;   // if true, the HWND is minimized due to a focus switch away when fullscreen mode
		bool  m_IgnoreSizeChange;           // if true, DXUT won't reset the device upon HWND size change

		double m_Time;                      // current time in seconds
		double m_AbsoluteTime;              // absolute time in seconds
		float m_ElapsedTime;                // time elapsed since last frame

		HINSTANCE m_HInstance;              // handle to the app instance
		double m_LastStatsUpdateTime;       // last time the stats were updated
		DWORD m_LastStatsUpdateFrames;      // frames count since last time the stats were updated
		float m_FPS;                        // frames per second
		int   m_CurrentFrameNumber;         // the current frame number
		HHOOK m_KeyboardHook;               // handle to keyboard hook
		bool  m_AllowShortcutKeysWhenFullscreen; // if true, when fullscreen enable shortcut keys (Windows keys, StickyKeys shortcut, ToggleKeys shortcut, FilterKeys shortcut) 
		bool  m_AllowShortcutKeysWhenWindowed;   // if true, when windowed enable shortcut keys (Windows keys, StickyKeys shortcut, ToggleKeys shortcut, FilterKeys shortcut) 
		bool  m_AllowShortcutKeys;          // if true, then shortcut keys are currently disabled (Windows key, etc)
		bool  m_CallDefWindowProc;          // if true, DXUTStaticWndProc will call DefWindowProc for unhandled messages. Applications rendering to a dialog may need to set this to false.
		STICKYKEYS m_StartupStickyKeys;     // StickyKey settings upon startup so they can be restored later
		TOGGLEKEYS m_StartupToggleKeys;     // ToggleKey settings upon startup so they can be restored later
		FILTERKEYS m_StartupFilterKeys;     // FilterKey settings upon startup so they can be restored later

		bool  m_HandleEscape;               // if true, then DXUT will handle escape to quit
		bool  m_HandleAltEnter;             // if true, then DXUT will handle alt-enter to toggle fullscreen
		bool  m_HandlePause;                // if true, then DXUT will handle pause to toggle time pausing
		bool  m_ShowMsgBoxOnError;          // if true, then msgboxes are displayed upon errors
		bool  m_NoStats;                    // if true, then DXUTGetFrameStats() and DXUTGetDeviceStats() will return blank strings
		bool  m_ClipCursorWhenFullScreen;   // if true, then DXUT will keep the cursor from going outside the window when full screen
		bool  m_ShowCursorWhenFullScreen;   // if true, then DXUT will show a cursor when full screen
		bool  m_ConstantFrameTime;          // if true, then elapsed frame time will always be 0.05f seconds which is good for debugging or automated capture
		float m_TimePerFrame;               // the constant time per frame in seconds, only valid if m_ConstantFrameTime==true
		bool  m_WireframeMode;              // if true, then D3DRS_FILLMODE==D3DFILL_WIREFRAME else D3DRS_FILLMODE==D3DFILL_SOLID 
		bool  m_AutoChangeAdapter;          // if true, then the adapter will automatically change if the window is different monitor
		bool  m_WindowCreatedWithDefaultPositions; // if true, then CW_USEDEFAULT was used and the window should be moved to the right adapter
		int   m_ExitCode;                   // the exit code to be returned to the command line

		bool  m_DXUTInited;                 // if true, then DXUTInit() has succeeded
		bool  m_WindowCreated;              // if true, then DXUTCreateWindow() or DXUTSetWindow() has succeeded
		bool  m_DeviceCreated;              // if true, then DXUTCreateDevice() has succeeded

		bool  m_DXUTInitCalled;             // if true, then DXUTInit() was called
		bool  m_WindowCreateCalled;         // if true, then DXUTCreateWindow() or DXUTSetWindow() was called
		bool  m_DeviceCreateCalled;         // if true, then DXUTCreateDevice() was called

		bool  m_DeviceObjectsCreated;       // if true, then DeviceCreated callback has been called (if non-NULL)
		bool  m_DeviceObjectsReset;         // if true, then DeviceReset callback has been called (if non-NULL)
		bool  m_InsideDeviceCallback;       // if true, then the framework is inside an app device callback
		bool  m_InsideMainloop;             // if true, then the framework is inside the main loop
		bool  m_Active;                     // if true, then the app is the active top level window
		bool  m_TimePaused;                 // if true, then time is paused
		bool  m_RenderingPaused;            // if true, then rendering is paused
		int   m_PauseRenderingCount;        // pause rendering ref count
		int   m_PauseTimeCount;             // pause time ref count
		bool  m_DeviceLost;                 // if true, then the device is lost and needs to be reset
		bool  m_NotifyOnMouseMove;          // if true, include WM_MOUSEMOVE in mousecallback
		bool  m_Automation;                 // if true, automation is enabled
		bool  m_InSizeMove;                 // if true, app is inside a WM_ENTERSIZEMOVE
		UINT  m_TimerLastID;                // last ID of the DXUT timer
		bool  m_MessageWhenD3D11NotAvailable;

		D3D_FEATURE_LEVEL  m_OverrideForceFeatureLevel; // if != -1, then overrid to use a featurelevel
		WCHAR m_ScreenShotName[256];        // command line screen shot name
		bool m_SaveScreenShot;              // command line save screen shot
		bool m_ExitAfterScreenShot;         // command line exit after screen shot

		int   m_OverrideAdapterOrdinal;         // if != -1, then override to use this adapter ordinal
		bool  m_OverrideWindowed;               // if true, then force to start windowed
		int   m_OverrideOutput;                 // if != -1, then override to use the particular output on the adapter
		bool  m_OverrideFullScreen;             // if true, then force to start full screen
		int   m_OverrideStartX;                 // if != -1, then override to this X position of the window
		int   m_OverrideStartY;                 // if != -1, then override to this Y position of the window
		int   m_OverrideWidth;                  // if != 0, then override to this width
		int   m_OverrideHeight;                 // if != 0, then override to this height
		bool  m_OverrideForceHAL;               // if true, then force to HAL device (failing if one doesn't exist)
		bool  m_OverrideForceREF;               // if true, then force to REF device (failing if one doesn't exist)
		bool  m_OverrideForceWARP;              // if true, then force to WARP device (failing if one doesn't exist)
		bool  m_OverrideConstantFrameTime;      // if true, then force to constant frame time
		float m_OverrideConstantTimePerFrame;   // the constant time per frame in seconds if m_OverrideConstantFrameTime==true
		int   m_OverrideQuitAfterFrame;         // if != 0, then it will force the app to quit after that frame
		int   m_OverrideForceVsync;             // if == 0, then it will force the app to use D3DPRESENT_INTERVAL_IMMEDIATE, if == 1 force use of D3DPRESENT_INTERVAL_DEFAULT
		bool  m_AppCalledWasKeyPressed;         // true if the app ever calls DXUTWasKeyPressed().  Allows for optimzation
		bool  m_ReleasingSwapChain;             // if true, the app is releasing its swapchain
		bool  m_IsInGammaCorrectMode;           // Tell DXUTRes and DXUTMisc that we are in gamma correct mode

		LPDXUTCALLBACKMODIFYDEVICESETTINGS      m_ModifyDeviceSettingsFunc;     // modify Direct3D device settings callback
		LPDXUTCALLBACKDEVICEREMOVED             m_DeviceRemovedFunc;            // Direct3D device removed callback
		LPDXUTCALLBACKFRAMEMOVE                 m_FrameMoveFunc;                // frame move callback
		LPDXUTCALLBACKKEYBOARD                  m_KeyboardFunc;                 // keyboard callback
		LPDXUTCALLBACKMOUSE                     m_MouseFunc;                    // mouse callback
		LPDXUTCALLBACKMSGPROC                   m_WindowMsgFunc;                // window messages callback

		LPDXUTCALLBACKISD3D11DEVICEACCEPTABLE   m_IsD3D11DeviceAcceptableFunc;  // D3D11 is device acceptable callback
		LPDXUTCALLBACKD3D11DEVICECREATED        m_D3D11DeviceCreatedFunc;       // D3D11 device created callback
		LPDXUTCALLBACKD3D11SWAPCHAINRESIZED     m_D3D11SwapChainResizedFunc;    // D3D11 SwapChain reset callback
		LPDXUTCALLBACKD3D11SWAPCHAINRELEASING   m_D3D11SwapChainReleasingFunc;  // D3D11 SwapChain lost callback
		LPDXUTCALLBACKD3D11DEVICEDESTROYED      m_D3D11DeviceDestroyedFunc;     // D3D11 device destroyed callback
		LPDXUTCALLBACKD3D11FRAMERENDER          m_D3D11FrameRenderFunc;         // D3D11 frame render callback

		void* m_ModifyDeviceSettingsFuncUserContext;     // user context for modify Direct3D device settings callback
		void* m_DeviceRemovedFuncUserContext;            // user context for Direct3D device removed callback
		void* m_FrameMoveFuncUserContext;                // user context for frame move callback
		void* m_KeyboardFuncUserContext;                 // user context for keyboard callback
		void* m_MouseFuncUserContext;                    // user context for mouse callback
		void* m_WindowMsgFuncUserContext;                // user context for window messages callback

		void* m_IsD3D11DeviceAcceptableFuncUserContext;  // user context for is D3D11 device acceptable callback
		void* m_D3D11DeviceCreatedFuncUserContext;       // user context for D3D11 device created callback
		void* m_D3D11SwapChainResizedFuncUserContext;    // user context for D3D11 SwapChain resized callback
		void* m_D3D11SwapChainReleasingFuncUserContext;  // user context for D3D11 SwapChain releasing callback
		void* m_D3D11DeviceDestroyedFuncUserContext;     // user context for D3D11 device destroyed callback
		void* m_D3D11FrameRenderFuncUserContext;         // user context for D3D11 frame render callback

		bool m_Keys[256];                                // array of key state
		bool m_LastKeys[256];                            // array of last key state
		bool m_MouseButtons[5];                          // array of mouse states

		std::vector<DXUT_TIMER>*  m_TimerList;           // list of DXUT_TIMER structs
		WCHAR m_StaticFrameStats[256];                   // static part of frames stats 
		WCHAR m_FPSStats[64];                            // fps stats
		WCHAR m_FrameStats[256];                         // frame stats (fps, width, etc)
		WCHAR m_DeviceStats[256];                        // device stats (description, device type, etc)
		WCHAR m_WindowTitle[256];                        // window title
	};

	STATE m_state;

public:
	DXUTState() { Create(); }
	~DXUTState() { Destroy(); }

	void Create()
	{
		g_bThreadSafe = true;
		(void)InitializeCriticalSectionAndSpinCount(&g_cs, 1000);

		ZeroMemory(&m_state, sizeof(STATE));
		m_state.m_OverrideStartX = -1;
		m_state.m_OverrideStartY = -1;
		m_state.m_OverrideForceFeatureLevel = (D3D_FEATURE_LEVEL)0;
		m_state.m_ScreenShotName[0] = 0;
		m_state.m_SaveScreenShot = false;
		m_state.m_ExitAfterScreenShot = false;
		m_state.m_OverrideAdapterOrdinal = -1;
		m_state.m_OverrideOutput = -1;
		m_state.m_OverrideForceVsync = -1;
		m_state.m_AutoChangeAdapter = true;
		m_state.m_ShowMsgBoxOnError = true;
		m_state.m_AllowShortcutKeysWhenWindowed = true;
		m_state.m_Active = true;
		m_state.m_CallDefWindowProc = true;
		m_state.m_HandleEscape = true;
		m_state.m_HandleAltEnter = true;
		m_state.m_HandlePause = true;
		m_state.m_IsInGammaCorrectMode = true;
		m_state.m_FPS = 1.0f;
		m_state.m_MessageWhenD3D11NotAvailable = true;
	}

	void Destroy()
	{
		SAFE_DELETE(m_state.m_TimerList);
		DXUTShutdown();
		DeleteCriticalSection(&g_cs);
	}

	// Macros to define access functions for thread safe access into m_state 
	GET_SET_ACCESSOR(DXUTDeviceSettings*, CurrentDeviceSettings);

	// D3D11 specific
	GET_SET_ACCESSOR(IDXGIFactory1*, DXGIFactory);
	GET_SET_ACCESSOR(IDXGIAdapter1*, DXGIAdapter);
	GET_SET_ACCESSOR(IDXGIOutput**, DXGIOutputArray);
	GET_SET_ACCESSOR(UINT, DXGIOutputArraySize);
	GET_SET_ACCESSOR(IDXGISwapChain*, DXGISwapChain);
	GETP_SETP_ACCESSOR(DXGI_SURFACE_DESC, BackBufferSurfaceDescDXGI);
	GET_SET_ACCESSOR(bool, RenderingOccluded);
	GET_SET_ACCESSOR(bool, DoNotStoreBufferSize);

	GET_SET_ACCESSOR(ID3D11Device*, D3D11Device);
	GET_SET_ACCESSOR(ID3D11DeviceContext*, D3D11DeviceContext);
	GET_SET_ACCESSOR(D3D_FEATURE_LEVEL, D3D11FeatureLevel);
	GET_SET_ACCESSOR(ID3D11Texture2D*, D3D11DepthStencil);
	GET_SET_ACCESSOR(ID3D11DepthStencilView*, D3D11DepthStencilView);
	GET_SET_ACCESSOR(ID3D11RenderTargetView*, D3D11RenderTargetView);
	GET_SET_ACCESSOR(ID3D11RasterizerState*, D3D11RasterizerState);

	GET_SET_ACCESSOR(ID3D11Device1*, D3D11Device1);
	GET_SET_ACCESSOR(ID3D11DeviceContext1*, D3D11DeviceContext1);

#ifdef USE_DIRECT3D11_2
	GET_SET_ACCESSOR(ID3D11Device2*, D3D11Device2);
	GET_SET_ACCESSOR(ID3D11DeviceContext2*, D3D11DeviceContext2);
#endif

	GET_SET_ACCESSOR(HWND, HWNDFocus);
	GET_SET_ACCESSOR(HWND, HWNDDeviceFullScreen);
	GET_SET_ACCESSOR(HWND, HWNDDeviceWindowed);
	GET_SET_ACCESSOR(HMONITOR, AdapterMonitor);
	GET_SET_ACCESSOR(HMENU, Menu);

	GET_SET_ACCESSOR(UINT, FullScreenBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT, FullScreenBackBufferHeightAtModeChange);
	GET_SET_ACCESSOR(UINT, WindowBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT, WindowBackBufferHeightAtModeChange);
	GETP_SETP_ACCESSOR(WINDOWPLACEMENT, WindowedPlacement);
	GET_SET_ACCESSOR(DWORD, WindowedStyleAtModeChange);
	GET_SET_ACCESSOR(bool, TopmostWhileWindowed);
	GET_SET_ACCESSOR(bool, Minimized);
	GET_SET_ACCESSOR(bool, Maximized);
	GET_SET_ACCESSOR(bool, MinimizedWhileFullscreen);
	GET_SET_ACCESSOR(bool, IgnoreSizeChange);

	GET_SET_ACCESSOR(double, Time);
	GET_SET_ACCESSOR(double, AbsoluteTime);
	GET_SET_ACCESSOR(float, ElapsedTime);

	GET_SET_ACCESSOR(HINSTANCE, HInstance);
	GET_SET_ACCESSOR(double, LastStatsUpdateTime);
	GET_SET_ACCESSOR(DWORD, LastStatsUpdateFrames);
	GET_SET_ACCESSOR(float, FPS);
	GET_SET_ACCESSOR(int, CurrentFrameNumber);
	GET_SET_ACCESSOR(HHOOK, KeyboardHook);
	GET_SET_ACCESSOR(bool, AllowShortcutKeysWhenFullscreen);
	GET_SET_ACCESSOR(bool, AllowShortcutKeysWhenWindowed);
	GET_SET_ACCESSOR(bool, AllowShortcutKeys);
	GET_SET_ACCESSOR(bool, CallDefWindowProc);
	GET_SET_ACCESSOR(STICKYKEYS, StartupStickyKeys);
	GET_SET_ACCESSOR(TOGGLEKEYS, StartupToggleKeys);
	GET_SET_ACCESSOR(FILTERKEYS, StartupFilterKeys);

	GET_SET_ACCESSOR(bool, HandleEscape);
	GET_SET_ACCESSOR(bool, HandleAltEnter);
	GET_SET_ACCESSOR(bool, HandlePause);
	GET_SET_ACCESSOR(bool, ShowMsgBoxOnError);
	GET_SET_ACCESSOR(bool, NoStats);
	GET_SET_ACCESSOR(bool, ClipCursorWhenFullScreen);
	GET_SET_ACCESSOR(bool, ShowCursorWhenFullScreen);
	GET_SET_ACCESSOR(bool, ConstantFrameTime);
	GET_SET_ACCESSOR(float, TimePerFrame);
	GET_SET_ACCESSOR(bool, WireframeMode);
	GET_SET_ACCESSOR(bool, AutoChangeAdapter);
	GET_SET_ACCESSOR(bool, WindowCreatedWithDefaultPositions);
	GET_SET_ACCESSOR(int, ExitCode);

	GET_SET_ACCESSOR(bool, DXUTInited);
	GET_SET_ACCESSOR(bool, WindowCreated);
	GET_SET_ACCESSOR(bool, DeviceCreated);
	GET_SET_ACCESSOR(bool, DXUTInitCalled);
	GET_SET_ACCESSOR(bool, WindowCreateCalled);
	GET_SET_ACCESSOR(bool, DeviceCreateCalled);
	GET_SET_ACCESSOR(bool, InsideDeviceCallback);
	GET_SET_ACCESSOR(bool, InsideMainloop);
	GET_SET_ACCESSOR(bool, DeviceObjectsCreated);
	GET_SET_ACCESSOR(bool, DeviceObjectsReset);
	GET_SET_ACCESSOR(bool, Active);
	GET_SET_ACCESSOR(bool, RenderingPaused);
	GET_SET_ACCESSOR(bool, TimePaused);
	GET_SET_ACCESSOR(int, PauseRenderingCount);
	GET_SET_ACCESSOR(int, PauseTimeCount);
	GET_SET_ACCESSOR(bool, DeviceLost);
	GET_SET_ACCESSOR(bool, NotifyOnMouseMove);
	GET_SET_ACCESSOR(bool, Automation);
	GET_SET_ACCESSOR(bool, InSizeMove);
	GET_SET_ACCESSOR(UINT, TimerLastID);
	GET_SET_ACCESSOR(bool, MessageWhenD3D11NotAvailable);
	GET_SET_ACCESSOR(bool, AppCalledWasKeyPressed);

	GET_SET_ACCESSOR(D3D_FEATURE_LEVEL, OverrideForceFeatureLevel);
	GET_ACCESSOR(WCHAR*, ScreenShotName);
	GET_SET_ACCESSOR(bool, SaveScreenShot);
	GET_SET_ACCESSOR(bool, ExitAfterScreenShot);

	GET_SET_ACCESSOR(int, OverrideAdapterOrdinal);
	GET_SET_ACCESSOR(bool, OverrideWindowed);
	GET_SET_ACCESSOR(int, OverrideOutput);
	GET_SET_ACCESSOR(bool, OverrideFullScreen);
	GET_SET_ACCESSOR(int, OverrideStartX);
	GET_SET_ACCESSOR(int, OverrideStartY);
	GET_SET_ACCESSOR(int, OverrideWidth);
	GET_SET_ACCESSOR(int, OverrideHeight);
	GET_SET_ACCESSOR(bool, OverrideForceHAL);
	GET_SET_ACCESSOR(bool, OverrideForceREF);
	GET_SET_ACCESSOR(bool, OverrideForceWARP);
	GET_SET_ACCESSOR(bool, OverrideConstantFrameTime);
	GET_SET_ACCESSOR(float, OverrideConstantTimePerFrame);
	GET_SET_ACCESSOR(int, OverrideQuitAfterFrame);
	GET_SET_ACCESSOR(int, OverrideForceVsync);
	GET_SET_ACCESSOR(bool, ReleasingSwapChain);
	GET_SET_ACCESSOR(bool, IsInGammaCorrectMode);

	GET_SET_ACCESSOR(LPDXUTCALLBACKMODIFYDEVICESETTINGS, ModifyDeviceSettingsFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKDEVICEREMOVED, DeviceRemovedFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKFRAMEMOVE, FrameMoveFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKKEYBOARD, KeyboardFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKMOUSE, MouseFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKMSGPROC, WindowMsgFunc);

	GET_SET_ACCESSOR(LPDXUTCALLBACKISD3D11DEVICEACCEPTABLE, IsD3D11DeviceAcceptableFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKD3D11DEVICECREATED, D3D11DeviceCreatedFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKD3D11SWAPCHAINRESIZED, D3D11SwapChainResizedFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKD3D11SWAPCHAINRELEASING, D3D11SwapChainReleasingFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKD3D11DEVICEDESTROYED, D3D11DeviceDestroyedFunc);
	GET_SET_ACCESSOR(LPDXUTCALLBACKD3D11FRAMERENDER, D3D11FrameRenderFunc);

	GET_SET_ACCESSOR(void*, ModifyDeviceSettingsFuncUserContext);
	GET_SET_ACCESSOR(void*, DeviceRemovedFuncUserContext);
	GET_SET_ACCESSOR(void*, FrameMoveFuncUserContext);
	GET_SET_ACCESSOR(void*, KeyboardFuncUserContext);
	GET_SET_ACCESSOR(void*, MouseFuncUserContext);
	GET_SET_ACCESSOR(void*, WindowMsgFuncUserContext);

	GET_SET_ACCESSOR(void*, IsD3D11DeviceAcceptableFuncUserContext);
	GET_SET_ACCESSOR(void*, D3D11DeviceCreatedFuncUserContext);
	GET_SET_ACCESSOR(void*, D3D11DeviceDestroyedFuncUserContext);
	GET_SET_ACCESSOR(void*, D3D11SwapChainResizedFuncUserContext);
	GET_SET_ACCESSOR(void*, D3D11SwapChainReleasingFuncUserContext);
	GET_SET_ACCESSOR(void*, D3D11FrameRenderFuncUserContext);

	GET_SET_ACCESSOR(std::vector<DXUT_TIMER>*, TimerList);
	GET_ACCESSOR(bool*, Keys);
	GET_ACCESSOR(bool*, LastKeys);
	GET_ACCESSOR(bool*, MouseButtons);
	GET_ACCESSOR(WCHAR*, StaticFrameStats);
	GET_ACCESSOR(WCHAR*, FPSStats);
	GET_ACCESSOR(WCHAR*, FrameStats);
	GET_ACCESSOR(WCHAR*, DeviceStats);
	GET_ACCESSOR(WCHAR*, WindowTitle);
};