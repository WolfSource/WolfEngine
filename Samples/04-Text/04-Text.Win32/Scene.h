/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : Scene.h
	Description		 : The main class of your game
	Comment          : This sample shows how to draw text in wolf
*/


#pragma once

#include <W_Game.h>
#include "W_GraphicsResource\W_SpriteBatch.h"

class Scene sealed : public Wolf::Framework::W_Game
{
public:
	Scene();
	virtual ~Scene();

	/*
		Allows the game to perform any initialization and it needs to before starting to run.
		Calling Game::Initialize() will enumerate through any components and initialize them as well.
		The parameter pOutputWindowsInfo represents the information of output window(s) of this game.
	*/
	void Initialize(std::map<int, std::vector<W_WindowInfo>> pOutputWindowsInfo) override;

	//The function "Load()" will be called once per game and is the place to load all of your game assets.
	void Load() override;

	//This is the place where allows the game to run logic such as updating the world, checking camera, collisions, physics, input, playing audio and etc.
	void Update(const Wolf::System::W_GameTime& pGameTime) override;
	
	//This is called when the game should draw itself.
	void Render(const Wolf::System::W_GameTime& pGameTime) override;

	//This is called when the window game should resized.
	void OnWindowResize(UINT pIndex) override;

	//This is called when the we lost graphics device.
	void OnDeviceLost() override;

	//Release will be called once per game and is the place to unload assets and release all resources
	ULONG Release() override;

private:
	std::unique_ptr<Wolf::Graphics::W_SpriteBatch> spriteBatch;
	std::unique_ptr<Wolf::Graphics::W_SpriteFont> spriteFont;
};

