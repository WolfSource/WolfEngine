//
// MainPage.xaml.cpp
// Declaration of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace Wolf_WinRT;

using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

MainPage::MainPage() : windowVisible(true), coreInput(nullptr), isReleased(false)
{
	InitializeComponent();

	// Register event handlers for page life cycle.
	CoreWindow^ window = Window::Current->CoreWindow;

	this->renderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &MainPage::OnRendering));

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &MainPage::OnVisibilityChanged);

	auto currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &MainPage::OnDisplayContentsInvalidated);

	this->swapChainPanel->CompositionScaleChanged += 
		ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &MainPage::OnCompositionScaleChanged);

	this->swapChainPanel->SizeChanged +=
		ref new SizeChangedEventHandler(this, &MainPage::OnSwapChainPanelSizeChanged);

	// Register our SwapChainPanel to get independent input pointer events
	auto workItemHandler = ref new WorkItemHandler([this] (IAsyncAction ^)
	{
		// The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
		this->coreInput = swapChainPanel->CreateCoreIndependentInputSource(
			Windows::UI::Core::CoreInputDeviceTypes::Mouse |
			Windows::UI::Core::CoreInputDeviceTypes::Touch |
			Windows::UI::Core::CoreInputDeviceTypes::Pen
			);

		// Register for pointer events, which will be raised on the background thread.
		this->coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerPressed);
		this->coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerMoved);
		this->coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerReleased);

		// Begin processing input messages as they're delivered.
		this->coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
	});

	// Run task on a dedicated high priority background thread.
	this->inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

	this->game = make_unique<Game>();
	this->game->Initialize(window, this->swapChainPanel);
}

MainPage::~MainPage()
{
	this->isReleased = true;
	// Stop rendering and processing events on destruction.
	this->game->Release();
	this->coreInput->Dispatcher->StopProcessEvents();
}

// Saves the current state of the app for suspend and terminate events.
void MainPage::SaveInternalState(IPropertySet^ state)
{
	//critical_section::scoped_lock lock(m_main->GetCriticalSection());
	//m_deviceResources->Trim();

	// Stop rendering when the app is suspended.
	//m_main->StopRenderLoop();

	// Put code to save app state here.
}

// Loads the current state of the app for resume events.
void MainPage::LoadInternalState(IPropertySet^ state)
{
	// Put code to load app state here.

	// Start rendering when the app is resumed.
	//m_main->StartRenderLoop();
}

// Window event handlers.

void MainPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	//m_windowVisible = args->Visible;
	//if (m_windowVisible)
	//{
	//	m_main->StartRenderLoop();
	//}
	//else
	//{
	//	m_main->StopRenderLoop();
	//}
}

// DisplayInformation event handlers.

void MainPage::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	//critical_section::scoped_lock lock(m_main->GetCriticalSection());
	//m_deviceResources->SetDpi(sender->LogicalDpi);
	//m_main->CreateWindowSizeDependentResources();
}

void MainPage::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	//critical_section::scoped_lock lock(m_main->GetCriticalSection());
	//m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
	//m_main->CreateWindowSizeDependentResources();
}


void MainPage::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	//critical_section::scoped_lock lock(m_main->GetCriticalSection());
	//m_deviceResources->ValidateDevice();
}

void MainPage::OnPointerPressed(Object^ sender, PointerEventArgs^ e)
{
	// When the pointer is pressed begin tracking the pointer movement.
	//m_main->StartTracking();
}

void MainPage::OnPointerMoved(Object^ sender, PointerEventArgs^ e)
{
	// Update the pointer tracking code.
	/*if (m_main->IsTracking())
	{
		m_main->TrackingUpdate(e->CurrentPoint->Position.X);
	}*/
}

void MainPage::OnPointerReleased(Object^ sender, PointerEventArgs^ e)
{
	// Stop tracking pointer movement when the pointer is released.
	//m_main->StopTracking();
}

void MainPage::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
	//critical_section::scoped_lock lock(m_main->GetCriticalSection());
	//m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
	//m_main->CreateWindowSizeDependentResources();
}

void MainPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	//critical_section::scoped_lock lock(m_main->GetCriticalSection());
	//m_deviceResources->SetLogicalSize(e->NewSize);
	//m_main->CreateWindowSizeDependentResources();
}

void MainPage::OnRendering(Platform::Object^ sender, Platform::Object^ args)
{
	if (!this->isReleased && this->windowVisible)
	{
		this->game->Run();
	}
	else
	{
		// App is in an inactive state so disable the OnRendering callback
		// This optimizes for power and allows the framework to become more queiecent
		if (this->renderingEventToken.Value != 0)
		{
			CompositionTarget::Rendering::remove(this->renderingEventToken);
			this->renderingEventToken.Value = 0;
		}
	}
}
