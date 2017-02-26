#pragma once

#include <w_game_time.h>
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"

// Renders Direct3D content on the screen.
namespace Test_DirectX12_Universal
{
	class scene
	{
	public:
		scene();
		void CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Update();
		bool Render();

		void OnWindowSizeChanged();
		void OnSuspending();
		void OnResuming();
		void OnDeviceRemoved();

	private:
		// TODO: Replace with your own content renderers.
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;

		// Rendering loop timer.
		wolf::system::w_game_time _game_time;
	};
}