#include "PCH.h"
#include "Scene.h"

using namespace std;
using namespace DirectX;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D::Shapes;

Scene::Scene()
{
	W_Game::SetAppName(L"04-Text.Win32");
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


	//Load sprite font style
	W_SpriteFontStyle spriteFontStyle =
	{
		L"Arial",
		DWRITE_FONT_WEIGHT_DEMI_BOLD,
		DWRITE_FONT_STYLE_ITALIC,
		DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		32.0,
		L"en-US"
	};
	this->spriteFont = make_unique<W_SpriteFont>(gDevice, spriteFontStyle);
	this->spriteFont->Load();

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
	auto pos = XMFLOAT2(100, 100);
	this->spriteBatch->Begin();
	{
		this->spriteBatch->DrawString(L"Hello Wolf Engine", &pos, this->spriteFont.get());
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
	UNIQUE_RELEASE(this->spriteFont);

	return W_Game::Release();
}
