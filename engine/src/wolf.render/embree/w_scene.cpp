#include "w_embree_pch.h"
#include "w_scene.h"
#include <future>

using namespace std;
using namespace wolf;
using namespace wolf::render::embree;

w_scene::w_scene(_In_z_ const std::wstring& pAppName,
                 _In_z_ const std::wstring& pContentPath) :
	exiting(false),
    _app_name(pAppName)
{
    this->load_state = LOAD_STATE::NOTLOADED;
	_super::set_class_name("w_scene");
	content_path = pContentPath;
}

w_scene::~w_scene()
{
    release();
}

W_RESULT w_scene::initialize(_In_ w_present_info& pPresentInfo)
{
    W_RESULT _hr = W_PASSED;
    if(w_graphic_device_manager::initialize(this->_gDevice, pPresentInfo) == W_PASSED)
    {
        //create embree rtc scene from this device
        this->_rtc_scene = rtcNewScene(this->_gDevice->get_device());
        if (!this->_rtc_scene)
        {
            _hr = W_FAILED;
        }
    }
    else
    {
        _hr = W_FAILED;
    }
    return _hr;
}

void w_scene::load()
{
}

void w_scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
	W_UNUSED(pGameTime);
}

W_RESULT w_scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    return W_PASSED;//w_graphics_device_manager::present();
}

void w_scene::on_resized(_In_ const w_point& pNewSize)
{
    
}

void w_scene::on_device_lost()
{
    
}

bool w_scene::run(_In_ w_present_info& pPresentInfos)
{
	if (this->load_state == LOAD_STATE::NOTLOADED)
	{
		this->load_state = LOAD_STATE::LOADING;

		//async initialize & load
		auto f = std::async(std::launch::async, [&]()->void
		{
			if(initialize(pPresentInfos) == W_PASSED)
            {
                load();
                this->load_state = LOAD_STATE::LOADED;
            }
            else
            {
                this->load_state = LOAD_STATE::ERROR;
            }
		});
		const std::chrono::milliseconds _milli_sec(16);
		f.wait_for(_milli_sec);
	}

    if (this->load_state != LOAD_STATE::LOADED) return true;

    update(this->_game_time);

    this->_game_time.tick([&]()
    {
//        if (w_graphics_device_manager::prepare() == W_PASSED)
//        {
//            render(this->_game_time);
//        }
    });
	
	//reset keyboard buffers for next cycle
	inputs_manager.reset_keyboard_buffers();

    return !this->exiting;
}

void w_scene::exit()
{
    this->exiting = true;
}

ULONG w_scene::release()
{
    if (_super::get_is_released()) return 1;
	
    if (this->_rtc_scene)
    {
        rtcReleaseScene(this->_rtc_scene);
    }
    this->exiting = true;

	return  _super::release();
}
