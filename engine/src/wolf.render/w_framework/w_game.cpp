#include "w_render_pch.h"
#include "w_game.h"
#include <future>

using namespace std;
//using namespace wolf::graphics;
using namespace wolf::framework;

#if defined(__WIN32)
w_game::w_game(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
#elif defined(__UWP)
w_game::w_game(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
#else
w_game::w_game(_In_z_ const std::string& pRunningDirectory, _In_z_ const std::string& pAppName) :
#endif
	exiting(false),
        _app_name(pAppName)
{
	_super::set_class_name("w_game");
	this->loadState = LoadState::NOTLOADED;

#ifdef __UWP
	logger.initialize(this->_app_name);
#else
    logger.initialize(this->_app_name, pRunningDirectory);
#endif
}

w_game::~w_game()
{
}

void w_game::initialize(map<int, vector<w_window_info>> pOutputWindowsInfo)
{	
	w_graphics_device_manager::initialize(pOutputWindowsInfo);

	//initialize sprite batch
	//auto _gDevice = get_graphics_device();

#ifdef __DX11_X__
	this->sprite_batch = make_unique<w_sprite_batch>(_gDevice);
	this->sprite_batch->load();
#endif
}

void w_game::load()
{
}

void w_game::update(const wolf::system::w_game_time& pGameTime)
{
	W_UNUSED(pGameTime);
}

HRESULT w_game::render(const wolf::system::w_game_time& pGameTime)
{
#ifdef	__DX11__
    
#pragma region show printf on screen log messages
    
    auto _msgs = logger.get_buffer();
    auto _size = _msgs.size();
    if (_size != 0)
    {
        this->sprite_batch->begin();
        {
            std::wstring _final_msg = L"";
            for (size_t i = 0; i < _size; ++i)
            {
                _final_msg += _msgs.at(i) + L"\r\n";
            }
            this->sprite_batch->draw_string(_final_msg,
                                            D2D1::RectF(5, 5, static_cast<FLOAT>(get_window_width() / 2), static_cast<FLOAT>(get_window_height())));
        }
        this->sprite_batch->end();
        
        logger.clearf();
    }
    
#pragma endregion
    
#endif
        
    auto _hr = w_graphics_device_manager::submit();
    if (_hr) return _hr;
    
    return w_graphics_device_manager::present();
}

#ifdef __WIN32
//Handle input message procedure
HRESULT w_game::on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
    return S_FALSE;
}
#endif

bool w_game::run(map<int, vector<w_window_info>> pOutputWindowsInfo)
{
	if (this->loadState == LoadState::NOTLOADED)
	{
		this->loadState = LoadState::LOADING;

		//Async initialize & load
		auto f = std::async(std::launch::async, [this, pOutputWindowsInfo]()->void
		{
			initialize(pOutputWindowsInfo);
			load();
			this->loadState = LoadState::LOADED;
		});
		std::chrono::milliseconds _milli_sec(16);
		f.wait_for(_milli_sec);
	}

    if (this->loadState != LoadState::LOADED) return true;

    update(this->_game_time);

    this->_game_time.tick([&]()
    {
        render(this->_game_time);
    });

    return !this->exiting;
}

void w_game::exit(_In_ const int pExitCode)
{
    std::exit(pExitCode);
}

ULONG w_game::release()
{
    if (_super::get_is_released()) return 0;
	
    this->exiting = true;
    logger.release();

	return  _super::release();
}
