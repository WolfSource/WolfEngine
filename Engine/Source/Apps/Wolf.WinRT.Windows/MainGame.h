#pragma once

#include <Game.h>

class MainGame sealed : public Wolf::Framework::Game
{
public:
	MainGame();

	void Initilize();
	void Load() override;
	void Update(Wolf::System::GameTime pGameTime) override;
	void Render(Wolf::System::GameTime pGameTime) override;
	void OnWindowSize() override;
	void OnDeviceLost() override;
	
	ULONG Release() override;
};

