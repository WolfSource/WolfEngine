#include "w_system_pch.h"

#if !defined(__APPLE__) && !defined(__UWP) && !defined(__ANDROID)

#include "w_window.h"
#include <map>
#include <chrono>

#ifdef __WIN32
static std::map<std::wstring, std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)>> sMsgsProcFunctions;
#endif

w_window::w_window() :
#ifdef __WIN32
    _hInstance(NULL),
    _hwnd(NULL),
#elif defined(__linux) && !defined(__ANDROID)
    _xcb_con(nullptr),
    _xcb_screen(nullptr),
    _xcb_window(nullptr),
#endif
    _title(L"Wolf.Engine"),
    _icon(L""),
    _full_screen(false),
    _screen_width(800),
    _screen_height(600),
    _screen_posX(0),
    _screen_posY(0),
    _id(0),
    _close(false)
{
    set_fixed_time_step(true);
}

w_window::~w_window()
{
	release();
}

#ifdef __WIN32
static LRESULT CALLBACK MsgProc(HWND pHwnd, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	TCHAR _class_name[MAX_PATH];
	GetClassName(pHwnd, _class_name, MAX_PATH);
	
	std::wstring _w_class_name(_class_name);
	auto _message_proc_func = sMsgsProcFunctions.find(_w_class_name);
	if (_message_proc_func != sMsgsProcFunctions.end())
	{
		if (_message_proc_func->second != nullptr && _message_proc_func->second(pHwnd, pMessage, pWParam, pLParam) == S_OK) return S_OK;
	}
	return DefWindowProc(pHwnd, pMessage, pWParam, pLParam);
}

HRESULT w_window::initialize(std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> pMsgProcFunction)
{
    auto _iter = windows_frame_time_in_sec.find((uint32_t)this->_id);
    if (_iter == windows_frame_time_in_sec.end())
    {
        windows_frame_time_in_sec[(uint32_t)this->_id] = 0.0f;
    }
    else
    {
        _id = -1;
        logger.error("Window with following ID " + std::to_string(this->_id) + " already registered. You can not get the frame time of it");
    }

	//Unregister all
	this->_hInstance = NULL;
	this->_hwnd = NULL;
	this->_hdc = NULL;
	if (this->_class_name.empty())
	{
		this->_class_name = L"Wolf_Engine";
	}
	UnregisterClass(this->_class_name.c_str(), NULL);

	// Get the instance of this application.
	this->_hInstance = GetModuleHandle(NULL);

	sMsgsProcFunctions[this->_class_name] = pMsgProcFunction;

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
	wc.lpszClassName = this->_class_name.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Setup the screen
	DEVMODE dev_mode;

	if (this->_full_screen)
	{
		// Determine the resolution of the clients desktop screen and create full screen window
		std::memset(&dev_mode, 0, sizeof(dev_mode));

		dev_mode.dmSize = sizeof(dev_mode);
		dev_mode.dmPelsWidth = static_cast<unsigned long>(GetSystemMetrics(SM_CXSCREEN));
		dev_mode.dmPelsHeight = static_cast<unsigned long>(GetSystemMetrics(SM_CYSCREEN));
		dev_mode.dmBitsPerPel = 32;
		dev_mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Setup full screen mode
		ChangeDisplaySettings(&dev_mode, CDS_FULLSCREEN);
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
	this->_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		this->_class_name.c_str(),
		this->_class_name.c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		this->_screen_posX,
		this->_screen_posY,
		this->_screen_width,
		this->_screen_height,
		NULL,
		NULL,
		this->_hInstance,
		NULL);
	if (!this->_hwnd)
	{
		V(S_FALSE, L"creating window handle", this->name, 3, true, true);
		return S_FALSE;
	}
	
	this->_hdc = GetDC(this->_hwnd);

	// Show window up on the screen
	ShowWindow(this->_hwnd, SW_SHOW);
	SetForegroundWindow(this->_hwnd);
	SetFocus(this->_hwnd);

	// Hide/Show the mouse cursor.
	ShowCursor(true);
        
        return S_OK;
}

#elif defined(__linux) && !defined(__ANDROID)

HRESULT w_window::initialize()
{
    //open the connection to the X server
    this->_xcb_con = xcb_connect (NULL, NULL);
    //get the setup
    auto _setup = xcb_get_setup(this->_xcb_con);
    //get the default screen
    auto _iter_screen = xcb_setup_roots_iterator (_setup);
    if (!_iter_screen.data) return S_FALSE;
    
    this->_xcb_screen = _iter_screen.data;
    
     //ask for our window's Id
    this->_xcb_window = new xcb_window_t();
    *this->_xcb_window = xcb_generate_id (this->_xcb_con);
  
    //create the window
    uint32_t _window_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    
    uint32_t _values[2];
    _values[0] = this->_xcb_screen->white_pixel;
    _values[1] = XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
              XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
              XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
              XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE;
    xcb_create_window(this->_xcb_con,                           //connection
                     0,                                         //depth               
                     *this->_xcb_window,                        //window Id           
                     this->_xcb_screen->root,                   //parent window       
                     this->_screen_posX, this->_screen_posY,    //x, y                
                     this->_screen_width, this->_screen_height, //width, height       
                     10,                                        //border_width        
                     XCB_WINDOW_CLASS_INPUT_OUTPUT,             //class               
                     this->_xcb_screen->root_visual,            //visual              
                     _window_mask, _values);                    //masks
    
    //force xcb to send notification when window is closed
    xcb_intern_atom_cookie_t _cookie_protocol = xcb_intern_atom(this->_xcb_con, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* _reply = xcb_intern_atom_reply(this->_xcb_con, _cookie_protocol, 0);

    xcb_intern_atom_cookie_t _cookie_delete = xcb_intern_atom(this->_xcb_con, 0, 16, "WM_DELETE_WINDOW");
    this->_atom_wm_delete_window = xcb_intern_atom_reply(this->_xcb_con, _cookie_delete, 0);

    xcb_change_property(
            this->_xcb_con, 
            XCB_PROP_MODE_REPLACE,
            *this->_xcb_window, 
            (*_reply).atom, 
            4, 
            32, 
            1,
            &(*this->_atom_wm_delete_window).atom);
        
    //set the title of the window
    auto _title = std::string(this->_title.begin(),this->_title.end());
    xcb_change_property(
            this->_xcb_con, 
            XCB_PROP_MODE_REPLACE, 
            *this->_xcb_window,
            XCB_ATOM_WM_NAME, 
            XCB_ATOM_STRING, 
            8,
            _title.length(), 
            _title.c_str());
  
    //set the title of the window icon
    auto _icon = std::string(this->_icon.begin(),this->_icon.end());
    xcb_change_property(
            this->_xcb_con, 
            XCB_PROP_MODE_REPLACE,
            *this->_xcb_window,
            XCB_ATOM_WM_ICON_NAME,
            XCB_ATOM_STRING,
            8,
            _icon.length(),
            _icon.c_str());
    
    //map the window on the screen
    xcb_map_window(this->_xcb_con, *this->_xcb_window);
    
    xcb_flush(this->_xcb_con);
  
    return S_OK;
}

#endif

#ifdef __WIN32
void w_window::run(std::function<void(void)> const pFunc)
{
	MSG msg;
	std::memset(&msg, 0, sizeof(msg));

    while (true)
    {
        auto _start = std::chrono::high_resolution_clock::now();
        // Handle the windows messages.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        if (this->_close)
        {
            break;
        }
        pFunc();

        auto _end = std::chrono::high_resolution_clock::now();
        auto _delta = std::chrono::duration<double, std::milli>(_end - _start).count();
        if (this->_id != -1)
        {
            windows_frame_time_in_sec.at((uint32_t)this->_id) = (float)_delta / 1000.0f;
        }
    }
}

#elif defined(__linux) && !defined(__ANDROID)
void w_window::run(std::function<void(void)>& pFunc)
{
    xcb_generic_event_t* _e = nullptr;
    while(!this->_close)
    {
        while (_e = xcb_poll_for_event(this->_xcb_con)) //xcb_wait_for_event
        {
            switch (_e->response_type & ~0x80) 
            {
                case XCB_CLIENT_MESSAGE:
                {
                    if ((*(xcb_client_message_event_t*)_e).data.data32[0] == (*this->_atom_wm_delete_window).atom) 
                    {
			this->_close = true;
                    }
                    break;
                }
                case XCB_DESTROY_NOTIFY:
                {
                    this->_close = true;
                    break;
                }
                case XCB_EXPOSE: 
                {
                    auto _ev = (xcb_expose_event_t*)_e;
                    //this->_window = _ev->window;
                    this->_screen_posX = _ev->x;
                    this->_screen_posY = _ev->y;
                    this->_screen_width = _ev->width;
                    this->_screen_height = _ev->height;

                    break;
                }
                case XCB_BUTTON_PRESS: 
                {
                    auto _ev = (xcb_button_press_event_t *)_e;
                    switch (_ev->detail) 
                    {
                        case 4:
                        {
                            //printf ("Wheel Button up in window %ld, at coordinates (%d,%d)\n",
                            //ev->event, ev->event_x, ev->event_y);
                            break;
                        }
                        case 5:
                        {
                            //printf ("Wheel Button down in window %ld, at coordinates (%d,%d)\n",
                            //ev->event, ev->event_x, ev->event_y);
                            break;
                        }
                        default:
                        {
                            //printf ("Button %d pressed in window %ld, at coordinates (%d,%d)\n",
                            //ev->detail, ev->event, ev->event_x, ev->event_y);
                        }
                    }
                    break;
                }
                case XCB_BUTTON_RELEASE: 
                {
                    auto _ev = (xcb_button_release_event_t*)_e;
                    //print_modifiers(ev->state);
                    //printf ("Button %d released in window %ld, at coordinates (%d,%d)\n",
                    //ev->detail, ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_MOTION_NOTIFY: 
                {
                    auto _ev = (xcb_motion_notify_event_t*)_e;
                    //printf ("Mouse moved in window %ld, at coordinates (%d,%d)\n",
                    //ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_ENTER_NOTIFY: 
                {
                    auto _ev = (xcb_enter_notify_event_t*)_e;
                    //printf ("Mouse entered window %ld, at coordinates (%d,%d)\n",
                    //ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_LEAVE_NOTIFY: 
                {
                    auto _ev = (xcb_leave_notify_event_t*)_e;
                    //printf ("Mouse left window %ld, at coordinates (%d,%d)\n",
                    //ev->event, ev->event_x, ev->event_y);
                    break;
                }
                case XCB_KEY_PRESS: 
                {
                    auto _ev = (xcb_key_press_event_t *)_e;
                    //print_modifiers(ev->state);
                    //printf ("Key pressed %c in window %ld\n", ev->detail, ev->event);
                    break;
                }
                case XCB_KEY_RELEASE: 
                {
                    auto _ev = (xcb_key_release_event_t *)_e;
                    //print_modifiers(ev->state);
                    //printf ("Key released %d in window %ld\n", ev->detail, ev->event);
                    break;
                }
                default:
                {
                    //Unknown event type, ignore it
                    //printf("Unknown event: %d\n", e->response_type);
                    break;
                }
            }
            //free the generic event
            free(_e);
            
            if (this->_close) break;
        }
          
        pFunc();
    }
}
#endif

void w_window::close()
{
	this->_close = true;
}

ULONG w_window::release()
{
	if (get_is_released()) return 0;

#ifdef __WIN32	
        
	this->_hInstance = NULL;
	this->_hwnd = NULL;
	this->_hdc = NULL;

	UnregisterClass(this->_class_name.c_str(), NULL);
	this->_class_name.clear();
        
#elif defined(__linux) && !defined(__ANDROID)
        
        xcb_disconnect(this->_xcb_con);
        this->_xcb_con = nullptr;
        this->_xcb_screen = nullptr;
        delete this->_xcb_window;
        this->_xcb_window = nullptr;
        this->_atom_wm_delete_window = nullptr;
        
#endif

	return w_object::release();
}

#pragma region Setters

#ifdef __WIN32
void w_window::set_class_name(_In_ LPWSTR pValue)
{
	this->_class_name = pValue;
}
#endif

void w_window::set_id(_In_ const uint32_t& pValue)
{
    this->_id = (int)pValue;
}

void w_window::set_full_screen(bool pValue)
{
	this->_full_screen = pValue;
}

void w_window::set_fixed_time_step(bool pValue)
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

#ifdef __WIN32
LPCWSTR w_window::get_class_name() const
{
	return this->_class_name.c_str();
}

HDC w_window::get_HDC() const
{
	return this->_hdc;
}

HINSTANCE w_window::get_HINSTANCE() const
{
	return this->_hInstance;
}

HWND w_window::get_HWND() const
{
	return this->_hwnd;
}
#endif

const uint32_t w_window::get_id() const
{
    return this->_id;
}

UINT w_window::get_width() const
{
	return this->_screen_width;
}

UINT w_window::get_height() const
{
	return this->_screen_height;
}

bool w_window::get_fixed_time_step() const
{
	return this->_game_time.get_fixed_time_step();
}

bool w_window::get_full_screen() const
{
	return this->_full_screen;
}

#pragma endregion


#endif //__APPLE__
