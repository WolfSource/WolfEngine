#include "w_render_pch.h"
#include "w_game.h"
#include <future>

using namespace std;
//using namespace wolf::graphics;
using namespace wolf::framework;

w_game::w_game(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
	exiting(false),
    _app_name(pAppName)
{
	_super::set_class_name("w_game");
	this->load_state = LOAD_STATE::NOTLOADED;

    if (!logger.get_is_open())
    {
#ifdef __UWP
        logger.initialize(this->_app_name);
#else
        logger.initialize(this->_app_name, pRunningDirectory);
#endif
    }
}

w_game::~w_game()
{
}

void w_game::initialize(_In_ map<int, vector<w_window_info>> pOutputWindowsInfo)
{	
	w_graphics_device_manager::initialize(pOutputWindowsInfo);
}

void w_game::load()
{
}

void w_game::update(_In_ const wolf::system::w_game_time& pGameTime)
{
	W_UNUSED(pGameTime);
}

HRESULT w_game::render(_In_ const wolf::system::w_game_time& pGameTime)
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
            
    return w_graphics_device_manager::present();
}

bool w_game::run(_In_ map<int, vector<w_window_info>> pOutputWindowsInfo)
{
	if (this->load_state == LOAD_STATE::NOTLOADED)
	{
		this->load_state = LOAD_STATE::LOADING;

		//Async initialize & load
		auto f = std::async(std::launch::async, [this, pOutputWindowsInfo]()->void
		{
			initialize(pOutputWindowsInfo);
			load();
			this->load_state = LOAD_STATE::LOADED;
		});
		std::chrono::milliseconds _milli_sec(16);
		f.wait_for(_milli_sec);
	}

    if (this->load_state != LOAD_STATE::LOADED) return true;

    update(this->_game_time);

    this->_game_time.tick([&]()
    {
        if (w_graphics_device_manager::prepare() == S_OK)
        {
            render(this->_game_time);
        }
    });
	
	//reset keyboard buffers for next cycle
	inputs_manager.reset_keyboard_buffers();

    return !this->exiting;
}

void w_game::exit(_In_ const int pExitCode)
{
    std::exit(pExitCode);
}

ULONG w_game::release()
{
    if (_super::get_is_released()) return 1;
	
    this->exiting = true;

	return  _super::release();
}
