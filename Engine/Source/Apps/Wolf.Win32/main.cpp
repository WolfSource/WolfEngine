#include "Wolf.Win32.PCH.h"
#include "Scene.h"

using namespace std;

static std::unique_ptr<W_Window> window;
static std::unique_ptr<Scene> scene;

static void Release()
{
	UNIQUE_RELEASE(window);
	UNIQUE_RELEASE(scene);
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	//Initialize scene & window
	scene = make_unique<Scene>();
	window = make_unique<W_Window>();
	window->Initialize();

	//run the main loop
	map<int, vector<W_WindowInfo>> windowsInfo = { { 0, { { window->GetHWND(), window->GetWidth(), window->GetHeight() } } } };
	std::function<void(void)> run = [&windowsInfo]()->void
	{
		scene->Run(windowsInfo);
	};
	window->Run(run);

	//release all
	Release();

	return EXIT_SUCCESS;
}