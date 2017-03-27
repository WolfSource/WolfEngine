#pragma once

#include "pch.h"
#include "scene.h"

//Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
ref class app sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
	app();

	// IFrameworkView methods.
	virtual void Initialize(_In_ Windows::ApplicationModel::Core::CoreApplicationView^ pApplicationView);
	virtual void SetWindow(_In_ Windows::UI::Core::CoreWindow^ pWindow);
	virtual void Load(_In_ Platform::String^ pEntryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected:
	//Application lifecycle event handlers.
	void OnActivated(_In_ Windows::ApplicationModel::Core::CoreApplicationView^ pApplicationView, _In_ Windows::ApplicationModel::Activation::IActivatedEventArgs^ pArgs);
	void OnSuspending(_In_ Platform::Object^ pSender, _In_ Windows::ApplicationModel::SuspendingEventArgs^ pArgs);
	void OnResuming(_In_ Platform::Object^ sender, _In_ Platform::Object^ args);

	//Window event handlers.
	void OnWindowSizeChanged(_In_ Windows::UI::Core::CoreWindow^ pSender, _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ pArgs);
	void OnVisibilityChanged(_In_ Windows::UI::Core::CoreWindow^ pSender, _In_ Windows::UI::Core::VisibilityChangedEventArgs^ pArgs);
	void OnWindowClosed(_In_ Windows::UI::Core::CoreWindow^ pSender, _In_ Windows::UI::Core::CoreWindowEventArgs^ pArgs);

	//DisplayInformation event handlers.
	void OnDpiChanged(_In_ Windows::Graphics::Display::DisplayInformation^ pSender, _In_ Platform::Object^ pArgs);
	void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ pSender, _In_ Platform::Object^ pArgs);
	void OnDisplayContentsInvalidated(_In_ Windows::Graphics::Display::DisplayInformation^ pSender, _In_ Platform::Object^ pArgs);

private:
	bool _exiting;
	bool _is_window_visible;
	w_window_info _w_info;
	std::unique_ptr<scene> _scene;
};


ref class Window sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
