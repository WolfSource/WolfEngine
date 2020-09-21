#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Debugger.h"

using namespace std;
using namespace D2D1;
using namespace Wolf::Graphics;

W_Debugger::W_Debugger(const shared_ptr<W_GraphicsDevice>& gDevice)
{
	auto name = "DebugState";
	auto context = gDevice->context2D;

	HRESULT hr = context->CreateSolidColorBrush(ColorF(ColorF::Red), &this->RedSolidBrushColor);
	OnFailed(hr, "Could not create SolidColorBrush in FPS", name, false);

	hr = context->CreateSolidColorBrush(ColorF(ColorF::Yellow), &this->YellowSolidBrushColor);
	OnFailed(hr, "Could not create SolidColorBrush in FPS", name, false);

	hr = context->CreateSolidColorBrush(ColorF(ColorF::Lime), &this->LimeSolidBrushColor);
	OnFailed(hr, "Could not create SolidColorBrush in FPS", name, false);
}

W_Debugger::~W_Debugger()
{

}

ID2D1SolidColorBrush* W_Debugger::GetColorState()
{
	if (this->state == State::Normal)
	{
		return this->LimeSolidBrushColor.Get();
	}
	if (this->state == State::Warning)
	{
		return this->YellowSolidBrushColor.Get();
	}
	return this->RedSolidBrushColor.Get();
}

ULONG W_Debugger::Release()
{
	if (IsReleased()) return 0;

	COM_RELEASE(this->LimeSolidBrushColor);
	COM_RELEASE(this->YellowSolidBrushColor);
	COM_RELEASE(this->RedSolidBrushColor);

	return W_Object::Release();
}