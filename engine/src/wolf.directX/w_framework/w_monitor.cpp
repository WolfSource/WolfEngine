#include "w_directX_pch.h"
#include "w_monitor.h"

using namespace D2D1;
using namespace wolf::system;
using namespace wolf::framework;
using namespace wolf::graphics;

w_monitor::w_monitor(const std::shared_ptr<w_graphics_device>& gDevice)
{
	_super::set_class_name(typeid(this).name());

	auto context = gDevice->context_2D;

	HRESULT _hr = context->CreateSolidColorBrush(ColorF(ColorF::Red), &this->_red_solid_brush_color);
	V(_hr, L"Could not create SolidColorBrush in FPS", name, 2, false, true);

	_hr = context->CreateSolidColorBrush(ColorF(ColorF::Yellow), &this->_yellow_solid_brush_color);
	V(_hr, L"Could not create SolidColorBrush in FPS", name, 2, false, true);

	_hr = context->CreateSolidColorBrush(ColorF(ColorF::Lime), &this->_lime_solid_brush_color);
	V(_hr, L"Could not create SolidColorBrush in FPS", name, 2, false, true);

	this->cpuInfo = std::make_unique<w_cpu>();
	this->cpuInfo->initialize();
}

w_monitor::~w_monitor()
{

}

void w_monitor::printf(_In_ w_sprite_batch* pSpriteBatch, const w_game_time& pGameTime, DirectX::XMFLOAT2* const pPosition)
{
	this->cpuInfo->update();

	auto _fps = pGameTime.get_frames_per_second();
	auto _cpu = this->cpuInfo->get_cpu_percentage();

	auto _fpsMsg = L"fps : " + std::to_wstring(_fps);
	if (_fps > 40)
	{
		pSpriteBatch->draw_string(_fpsMsg, *pPosition, this->_lime_solid_brush_color.Get());
	}
	else if (_fps > 30)
	{
		pSpriteBatch->draw_string(_fpsMsg, *pPosition, this->_yellow_solid_brush_color.Get());
	}
	else
	{
		pSpriteBatch->draw_string(_fpsMsg, *pPosition, this->_red_solid_brush_color.Get());
	}

	pPosition->y += 15;

	auto _cpuMsg = L"cpu : " + std::to_wstring(_cpu) + L"%";
	if (_cpu > 80)
	{
		pSpriteBatch->draw_string(_cpuMsg, *pPosition, this->_red_solid_brush_color.Get());
	}
	else if (_cpu > 50)
	{
		pSpriteBatch->draw_string(_cpuMsg, *pPosition, this->_yellow_solid_brush_color.Get());
	}
	else
	{
		pSpriteBatch->draw_string(_cpuMsg, *pPosition, this->_lime_solid_brush_color.Get());
	}
}

ID2D1SolidColorBrush* w_monitor::get_colorState()
{
	if (this->state == State::Normal)
	{
		return this->_lime_solid_brush_color.Get();
	}
	if (this->state == State::Warning)
	{
		return this->_yellow_solid_brush_color.Get();
	}
	return this->_red_solid_brush_color.Get();
}

ULONG w_monitor::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_lime_solid_brush_color);
	COMPTR_RELEASE(this->_yellow_solid_brush_color);
	COMPTR_RELEASE(this->_red_solid_brush_color);

	UNIQUE_RELEASE(this->cpuInfo);

	return _super::release();
}