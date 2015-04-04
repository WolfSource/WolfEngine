#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Game.h"

using namespace std;
using namespace DirectX;
using namespace Wolf::Graphics;
using namespace Wolf::Framework;

W_Game::W_Game()
{
	this->loaded = false;
	this->gameTime.SetFixedTimeStep(true);
}

W_Game::~W_Game()
{
}

void W_Game::Initialize(map<int, vector<W_WindowInfo>> pOutputWindowsInfo)
{
	Logger.Initialize(this->appName);
	W_GraphicsDeviceManager::Initialize();
	W_GraphicsDeviceManager::InitializeOutputWindows(pOutputWindowsInfo);
}

void W_Game::Load()
{

}

void W_Game::Update(Wolf::System::W_GameTime pGameTime)
{
	
}

void W_Game::Render(Wolf::System::W_GameTime pGameTime)
{
	
}

bool W_Game::Run(map<int, vector<W_WindowInfo>> pOutputWindowsInfo)
{
	if (!this->loaded)
	{
		Initialize(pOutputWindowsInfo);
		Load();
		this->loaded = true;
	}
	Update(this->gameTime);

	this->gameTime.Tick([&]()
	{
		W_GraphicsDeviceManager::BeginRender();
		Render(this->gameTime);
		W_GraphicsDeviceManager::EndRender(nullptr);
	});

	return true;
}

ULONG W_Game::Release()
{
	if (this->IsReleased()) return 0;
	
	Logger.Release();
	return W_GraphicsDeviceManager::Release();
}
