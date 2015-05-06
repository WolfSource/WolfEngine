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
using namespace DirectX;
using namespace D2D1;
using namespace Wolf::System;
using namespace Wolf::Framework;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D::Shapes;

Scene::Scene()
{
	W_Game::SetAppName(L"02-Direct2DShapes.Win32");

	ColorF color = ColorF::Navy;
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

	//Load spriteBatch
	this->spriteBatch = make_unique<W_SpriteBatch>(gDevice);
	this->spriteBatch->Load();

	//Load ellipse
	this->ellipse = make_unique<W_Ellipse>(gDevice);
	this->ellipse->SetGeormetry(530, 100, 70, 70);
	this->ellipse->SetColor(ColorF::White);
	this->ellipse->SetBorderColor(ColorF::White);

	//Load rectangle
	this->rectangle = make_unique<W_Rectangle>(gDevice);
	this->rectangle->SetGeormetry(0, 300, this->GetWindowWidth(), 400, 0, 0);
	this->rectangle->SetColor(ColorF::Blue);
	this->rectangle->SetBorderColor(ColorF::Blue);

#pragma region Create Clouds

	this->geoCloud0 = make_unique<W_Geometry>(gDevice);
	this->geoCloud0->SetColor(ColorF::LightGray);
	this->geoCloud0->SetBorderColor(ColorF::LightGray);

	this->geoCloud0->Begin();
	{
		this->geoCloud0->AddArc(
			Point2F(40, 220),//Start
			D2D1::Point2F(210, 220),//End
			D2D1::SizeF(70, 70),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->geoCloud0->AddArc(
			Point2F(120, 220),//Start
			D2D1::Point2F(340, 220),//End
			D2D1::SizeF(100, 150),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->geoCloud0->AddArc(
			Point2F(150, 220),//Start
			D2D1::Point2F(400, 220),//End
			D2D1::SizeF(100, 90),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
	}
	this->geoCloud0->End();

	this->geoCloud1 = make_unique<W_Geometry>(gDevice);
	this->geoCloud1->SetColor(ColorF::LightGray);
	this->geoCloud1->SetBorderColor(ColorF::LightGray);
	this->geoCloud1->Begin();
	{
		this->geoCloud1->AddArc(
			Point2F(420, 220),//Start
			D2D1::Point2F(580, 220),//End
			D2D1::SizeF(20, 20),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->geoCloud1->AddArc(
			Point2F(520, 220),//Start
			D2D1::Point2F(770, 220),//End
			D2D1::SizeF(40, 50),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
	}
	this->geoCloud1->End();

#pragma endregion
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
		this->spriteBatch->Draw(this->ellipse.get());
		this->spriteBatch->Draw(this->rectangle.get());
		this->spriteBatch->Draw(this->geoCloud0.get());
		this->spriteBatch->Draw(this->geoCloud1.get());
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

	UNIQUE_RELEASE(this->ellipse);
	UNIQUE_RELEASE(this->rectangle);

	UNIQUE_RELEASE(this->geoCloud0);
	UNIQUE_RELEASE(this->geoCloud1);

	return W_Game::Release();
}
