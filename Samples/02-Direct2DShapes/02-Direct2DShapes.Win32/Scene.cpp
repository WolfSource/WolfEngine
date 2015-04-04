/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : Scene.cpp
	Description		 : This is the main type for your game
	Comment          :
*/


#include "PCH.h"
#include "Scene.h"

using namespace std;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D;

Scene::Scene() : width(400), height(200)
{
	W_Game::SetAppName("02-Direct2DShapes.Win32");
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

	//Load spriteBatch
	this->spriteBatch = make_unique<W_SpriteBatch>(gDevice);
	this->spriteBatch->Load();

	//Load rectangle2D
	this->rectangle2D = make_unique<W_Rectangle2D>(gDevice);
}

static float step = 1;
void Scene::Update(Wolf::System::W_GameTime pGameTime)
{
	// TODO: Add your update logic here

	this->rectangle2D->SetGeormetry(7, 7, width, height);
	if (height == GetWindowHeight() - 7 || height == 7)
	{
		step *= -1;
	}
	width = width + step;
	height = height + step;

	W_Game::Update(pGameTime);
}

void Scene::Render(Wolf::System::W_GameTime pGameTime)
{
	// TODO: Add your drawing code here
	this->spriteBatch->Begin();
	{
		this->spriteBatch->Draw(this->rectangle2D.get());
	}
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
	SMART_RELEASE(this->spriteBatch);
	SMART_RELEASE(this->rectangle2D);

	return W_Game::Release();
}
