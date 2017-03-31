#include "pch.h"
#include "app.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

using Microsoft::WRL::ComPtr;

app::app() :
	_exiting(false),
	_is_window_visible(true)
{

}

// The first method called when the IFrameworkView is being created.
void app::Initialize(_In_ CoreApplicationView^ pApplicationView)
{
	/*
		Register event handlers for app lifecycle. This example includes Activated, so that we
		can make the CoreWindow active and start rendering on the window.
	*/
	pApplicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &app::OnActivated);

	CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &app::OnSuspending);
	CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &app::OnResuming);
}

//Called when the CoreWindow object is created (or re-created).
void app::SetWindow(_In_ CoreWindow^ pWindow)
{
	auto _current_display_information = DisplayInformation::GetForCurrentView();

	_w_info.window = reinterpret_cast<IUnknown*>(pWindow);
	_w_info.window_size = pWindow->Bounds;
	_w_info.window_native_orientation = _current_display_information->NativeOrientation;
	_w_info.window_current_orientation = _current_display_information->CurrentOrientation;
	_w_info.window_dpi = _current_display_information->LogicalDpi;
	_w_info.support_high_resolutions = true;

	pWindow->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &app::OnWindowSizeChanged);
	pWindow->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &app::OnVisibilityChanged);
	pWindow->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &app::OnWindowClosed);

	auto _current_display_info = DisplayInformation::GetForCurrentView();
	_current_display_info->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &app::OnDpiChanged);
	_current_display_info->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &app::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &app::OnDisplayContentsInvalidated);
}

//Initializes scene resources, or loads a previously saved app state.
void app::Load(_In_ Platform::String^ pEntryPoint)
{
	if (this->_scene == nullptr)
	{
		this->_scene = std::make_unique<scene>();
	}
}

//This method is called after the window becomes active.
void app::Run()
{
	//run the main loop
	std::map<int, std::vector<w_window_info>> _windows_infos = { { 0,{ _w_info } } };

	while (!this->_exiting)
	{
		if (this->_is_window_visible)
		{
			this->_scene->run(_windows_infos);
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

/*
	Required for IFrameworkView.
	Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
	class is torn down while the app is in the foreground.
*/
void app::Uninitialize()
{
}

//Application lifecycle event handlers.
void app::OnActivated(_In_ CoreApplicationView^ pApplicationView, _In_ IActivatedEventArgs^ pArgs)
{
	//Run() won't start until the CoreWindow is activated.
	CoreWindow::GetForCurrentThread()->Activate();
}

void app::OnSuspending(_In_ Platform::Object^ pSender, _In_ SuspendingEventArgs^ pArgs)
{
	/*
		Save app state asynchronously after requesting a deferral. Holding a deferral
		indicates that the application is busy performing suspending operations. Be
		aware that a deferral may not be held indefinitely. After about five seconds,
		the app will be forced to exit.
	*/
}

void app::OnResuming(_In_ Platform::Object^ pSender, _In_ Platform::Object^ pArgs)
{
	/*
		Restore any data or state that was unloaded on suspend. By default, data
		and state are persisted when resuming from suspend. Note that this event
		does not occur if the app was previously terminated.
	*/
}

//Window event handlers.
void app::OnWindowSizeChanged(_In_ CoreWindow^ pSender, _In_ WindowSizeChangedEventArgs^ pArgs)
{
}

void app::OnVisibilityChanged(_In_ CoreWindow^ pSender, _In_ VisibilityChangedEventArgs^ pArgs)
{
	this->_is_window_visible = pArgs->Visible;
}

void app::OnWindowClosed(_In_ CoreWindow^ pSender, _In_ CoreWindowEventArgs^ pArgs)
{
	this->_exiting = true;
}

//DisplayInformation event handlers.
void app::OnDpiChanged(_In_ DisplayInformation^ pSender, _In_ Object^ pArgs)
{
	/*
		The value for LogicalDpi retrieved here may not match the effective DPI of the app
		if it is being scaled for high resolution devices.
	*/
}

void app::OnOrientationChanged(_In_ DisplayInformation^ pSender, _In_ Object^ pArgs)
{
}

void app::OnDisplayContentsInvalidated(DisplayInformation^ pSender, Object^ pArgs)
{
}

#pragma region Entery Main

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto _window = ref new Window();
	CoreApplication::Run(_window);
	return 0;
}

IFrameworkView^ Window::CreateView()
{
	return ref new app();
}

#pragma endregion