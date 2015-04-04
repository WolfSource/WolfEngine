#include "pch.h"
#include "MainGame.h"
#include <GraphicsResource/Model/WMesh.h>

using namespace std;
using namespace Wolf::Graphics;

MainGame::MainGame()
{
}

void MainGame::Load()
{
	//Load vertices
	const float v [] =
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
	};
	vector<float> vertices(v, v + sizeof(v) / sizeof(v[0]));
	
	//Load indices
	const UINT16 i[] = { 0, 1, 2, 0, 3, 1 };
	vector<UINT16> indices(i, i + sizeof(i) / sizeof(i[0]));

	auto mesh = new WMesh();
	mesh->SetName("Quad");
	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);


	auto sceneManager = this->GetSceneManager();
	sceneManager->AddMesh(mesh);


	Game::Load();
}

void MainGame::Update(Wolf::System::GameTime pGameTime)
{
	Game::Update(pGameTime);
}

void MainGame::Render(Wolf::System::GameTime pGameTime)
{
	Game::Render(pGameTime);
}

void MainGame::OnWindowSize()
{
	Game::OnWindowSize();
}

void MainGame::OnDeviceLost()
{
	Game::OnDeviceLost();
}

ULONG MainGame::Release()
{
	return Game::Release();
}
