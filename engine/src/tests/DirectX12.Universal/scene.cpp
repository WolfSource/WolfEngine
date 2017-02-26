#include "pch.h"
#include "scene.h"
#include "Common\DirectXHelper.h"

#include <w_logger.h> 
#include <w_convert.h>
#include <w_io.h>

using namespace Test_DirectX12_Universal;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// The DirectX 12 Application template is documented at http://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

// Loads and initializes application assets when the application is loaded.
scene::scene()
{
	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	//using namespace Windows::Storage;
	//auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
	//auto dir = folder->Path->ToString()->Data();
	//auto _a = wolf::system::io::get_is_directory("pooya");

	OutputDebugStringW(wolf::system::io::get_current_directoryW().c_str());

	//auto _app_folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

	//auto _t = create_task(_app_folder->GetFoldersAsync()).then([=](Windows::Foundation::Collections::IVector<Windows::Storage::StorageFolder^>^ itemsInFolder) 
	//{
	//	for (auto it = itemsInFolder->First(); it->HasCurrent; it->MoveNext())
	//	{
	//		auto _file = it->Current;
	//		auto output = _file->Name + (ref new Platform::String(L"\n"));
	//		OutputDebugString(output->Begin());
	//	}
	//});

	//wolf::system::io::get_directory_exists_async(Windows::ApplicationModel::Package::Current->InstalledLocation, "Asset").then([](_In_ BOOL pValue)
	//{
	//	OutputDebugString(pValue.ToString()->Data());
	//});
	//wolf::system::io::get_file_exists_async(Windows::ApplicationModel::Package::Current->InstalledLocation, "Wolf.System.Universal.dll").then([](_In_ BOOL pValue)
	//{
	//	OutputDebugString(pValue.ToString()->Data());
	//});
	

	
	//concurrency::when_all(tasks.begin(), tasks.end()).then([this]()
	//{	
	//	OutputDebugString(L"Done");
	//});

	logger.initialize(L"Test.DirectX12.Universal");
	logger.write("Pooya");
	
}

// Creates and initializes the renderers.
void scene::CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	// TODO: Replace this with your app's content initialization.
	m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(deviceResources));

	OnWindowSizeChanged();
}

// Updates the application state once per frame.
void scene::Update()
{
	// Update scene objects.
	_game_time.tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		m_sceneRenderer->Update(_game_time);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool scene::Render()
{
	// Don't try to render anything before the first Update.
	if (_game_time.get_frame_count() == 0)
	{
		return false;
	}

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	return m_sceneRenderer->Render();
}

// Updates application state when the window's size changes (e.g. device orientation change)
void scene::OnWindowSizeChanged()
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

// Notifies the app that it is being suspended.
void scene::OnSuspending()
{
	// TODO: Replace this with your app's suspending logic.

	// Process lifetime management may terminate suspended apps at any time, so it is
	// good practice to save any state that will allow the app to restart where it left off.

	m_sceneRenderer->SaveState();

	// If your application uses video memory allocations that are easy to re-create,
	// consider releasing that memory to make it available to other applications.
}

// Notifes the app that it is no longer suspended.
void scene::OnResuming()
{
	// TODO: Replace this with your app's resuming logic.
}

// Notifies renderers that device resources need to be released.
void scene::OnDeviceRemoved()
{
	// TODO: Save any necessary application or renderer state and release the renderer
	// and its resources which are no longer valid.
	m_sceneRenderer->SaveState();
	m_sceneRenderer = nullptr;
}
