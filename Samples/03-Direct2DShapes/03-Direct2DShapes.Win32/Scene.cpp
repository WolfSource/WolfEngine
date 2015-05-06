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
using namespace Wolf::Graphics::Direct2D::Shapes;

Scene::Scene() : width(400), height(200)
{
	W_Game::SetAppName(L"03-Direct2DShapes.Win32");
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

	//Load rectangle
	this->rectangle2D = make_unique<W_Rectangle>(gDevice);
	this->rectangle2D->SetGeormetry(7, 7, width, height, 0, 0);
	this->rectangle2D->SetColor(D2D1::ColorF(0.176f, 0.52f, 0.93f));
	this->rectangle2D->SetBorderColor(D2D1::ColorF::White);

	//Load rounded rectangle
	this->roundedRectangle2D = make_unique<W_Rectangle>(gDevice);
	this->roundedRectangle2D->SetGeormetry(450, 150, width / 2, height , 50, 50);
	this->roundedRectangle2D->SetColor(D2D1::ColorF::Purple);
	this->roundedRectangle2D->SetBorderColor(D2D1::ColorF::White);

	//Load ellipse
	this->ellipse = make_unique<W_Ellipse>(gDevice);
	this->ellipse->SetGeormetry(400, 400, 20, 20);
	this->ellipse->SetColor(D2D1::ColorF::Wheat);
	this->ellipse->SetBorderColor(D2D1::ColorF::White);

	//Load line
	auto startPointLine = D2D1_POINT_2F();
	startPointLine.x = 220;
	startPointLine.y = 220;

	auto stopPointLine = D2D1_POINT_2F();
	stopPointLine.x = 350;
	stopPointLine.y = 350;

	this->line = make_unique<W_Line>(gDevice, startPointLine, stopPointLine);
	this->line->SetColor(D2D1::ColorF::Lime);
	this->line->SetBorderColor(D2D1::ColorF::Lime);
}

void Scene::Update(const Wolf::System::W_GameTime& pGameTime)
{
	// TODO: Add your update logic here
	W_Game::Update(pGameTime);
}

void Scene::Render(const Wolf::System::W_GameTime& pGameTime)
{
	// TODO: Add your drawing code here
	this->spriteBatch->Begin();
	{
		this->spriteBatch->Draw(this->rectangle2D.get());
		this->spriteBatch->Draw(this->roundedRectangle2D.get());
		this->spriteBatch->Draw(this->ellipse.get());
		this->spriteBatch->Draw(this->line.get());
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
	UNIQUE_RELEASE(this->spriteBatch);
	UNIQUE_RELEASE(this->rectangle2D);
	UNIQUE_RELEASE(this->roundedRectangle2D);
	UNIQUE_RELEASE(this->ellipse);
	UNIQUE_RELEASE(this->line);

	return W_Game::Release();
}
