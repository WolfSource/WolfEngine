#include "w_render_pch.h"
#include "w_game.h"
#include <future>

using namespace std;
//using namespace wolf::graphics;
using namespace wolf::framework;

#if defined(__linux) || defined(__ANDROID) || defined(__APPLE__)
std::string w_game::_content_directory_path = "";
#else
std::wstring w_game::_content_directory_path = L"";
#endif

w_game::w_game() : 
	exiting(false)
{
	_super::set_class_name("w_game");//typeid(this).name() not supported in NDK
	this->loadState = LoadState::NOTLOADED;

#if defined(__linux) || defined(__ANDROID) ||  defined(__APPLE__)
    logger.initialize(this->_app_name, wolf::system::io::get_current_directory());
    _content_directory_path = wolf::system::io::get_content_directory();
#else

#ifdef __WIN32
    logger.initialize(this->_app_name, wolf::system::io::get_current_directoryW());
#elif defined (__UWP)
	logger.initialize(this->_app_name);
#endif

    _content_directory_path = wolf::system::io::get_content_directoryW();
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

void w_game::begin_render(const wolf::system::w_game_time& pGameTime)
{
	w_graphics_device_manager::begin_render();
}

void w_game::render(const wolf::system::w_game_time& pGameTime)
{
#ifdef	__DX11_X__
	//execute all command lists
	auto _gDevice = get_graphics_device();
	auto _size = _gDevice->command_queue.size();
	if (_size > 0)
	{
        for (size_t i = 0; i < _size; ++i)
        {
            auto _command = _gDevice->command_queue.at(i);
            if (_command)
            {
                _gDevice->context->ExecuteCommandList(_command, true);
                _command->Release();
                _command = nullptr;
            }
        }
	}
#endif
}

void w_game::end_render(const wolf::system::w_game_time& pGameTime)
{
#ifdef	__DX11_X__

#pragma region Show Printf on screen log messages

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

	w_graphics_device_manager::end_render();
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
        begin_render(this->_game_time);
        render(this->_game_time);
        end_render(this->_game_time);
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
