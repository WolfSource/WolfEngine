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
	OnLoadCompleted(0);
}

void W_Game::Update(const Wolf::System::W_GameTime& pGameTime)
{
	
}

void W_Game::BeginRender()
{
	W_GraphicsDeviceManager::BeginRender();
}

void W_Game::Render(const Wolf::System::W_GameTime& pGameTime)
{
	
}

void W_Game::EndRender()
{
	W_GraphicsDeviceManager::EndRender();
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
		BeginRender();
		Render(this->gameTime);
		EndRender();
	});

	return true;
}

ULONG W_Game::Release()
{
	if (this->IsReleased()) return 0;
	
	Logger.Release();
	return W_GraphicsDeviceManager::Release();
}
