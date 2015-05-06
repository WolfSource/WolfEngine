#include "PCH.h"
#include <W_Window.h>

using namespace std;

static std::unique_ptr<W_Window> window;

static void Release()
{
	UNIQUE_RELEASE(window);
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	//Initialize scene & window
	window = make_unique<W_Window>();
	window->Initialize();

	//run the main loop
	std::function<void(void)> run = []()->void
	{
		//NOP
	};
	window->Run(run);

	//release all
	Release();

	return EXIT_SUCCESS;
}