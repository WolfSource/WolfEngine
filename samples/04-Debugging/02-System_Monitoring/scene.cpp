#include "PCH.h"
#include "Scene.h"

using namespace std;
using namespace DirectX;
using namespace D2D1;
using namespace Wolf::System;
using namespace Wolf::Framework;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D::Shapes;

Scene::Scene()
{
	W_Game::SetAppName(L"08-Encoding_Texture.Win32");

	ColorF color = ColorF::Black;
	this->backColor[0] = color.r;
	this->backColor[1] = color.g;
	this->backColor[2] = color.b;
}

Scene::~Scene()
{
	Release();
}

void Scene::Initialize(std::map<int, std::vector<W_WindowInfo>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	W_Game::Initialize(pOutputWindowsInfo);
}

void Scene::Load()
{
	// TODO: load your game assets here
	auto gDevice = this->GDevice();

	this->monitor = make_unique<W_Monitor>(gDevice);

	W_Game::Load();
}

void Scene::Update(const Wolf::System::W_GameTime& pGameTime)
{
	// TODO: Add your update logic here

	W_Game::Update(pGameTime);
}

void Scene::Render(const Wolf::System::W_GameTime& pGameTime)
{
	// TODO: Add your drawing code here

	auto pos = XMFLOAT2(5, 5);

	this->spriteBatch->Begin();
	this->monitor->Printf(this->spriteBatch.get(), pGameTime, &pos);
	this->spriteBatch->End();

	W_Game::Render(pGameTime);
}

void Scene::OnWindowResize(UINT pIndex)
{
	// TODO: Add your logic for on window resizing event

	W_Game::OnWindowResize(pIndex);
}

void Scene::OnDeviceLost()
{
	// TODO: Add your logic for on device lost event

	W_Game::OnDeviceLost();
}

ULONG Scene::Release()
{
	if (this->IsReleased()) return 0;

	// TODO: Release your assets here
	UNIQUE_RELEASE(this->monitor);

	return W_Game::Release();
}
