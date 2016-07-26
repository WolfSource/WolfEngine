/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_rectangle.h
	Description		 : The Rectangle
	Comment          :
*/

#ifndef _W_RECTANGLE_H_
#define _W_RECTANGLE_H_

#include <Windows.h>

namespace wolf
{
	namespace framework
	{
		struct w_rectangle : public RECT
		{
			DLL w_monitor(const std::shared_ptr<graphics::w_graphics_device>& gDevice);
			DLL virtual ~w_monitor();

			//Print information of monitoring on the screen
			DLL void printf(_In_ graphics::w_sprite_batch* pSpriteBatch, const system::w_game_time& pGameTime, _In_ DirectX::XMFLOAT2* const pPosition);

			//Release all resources
			DLL ULONG release() override;

#pragma region Getters
			//Get color state - Normal = Lime, Warning = Yellow, Critical = Red
			ID2D1SolidColorBrush* get_colorState();
#pragma endregion

		private:
			typedef system::w_object					 _super;
			enum State { Normal, Warning, Error };
			State state;

			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> _red_solid_brush_color;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> _yellow_solid_brush_color;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> _lime_solid_brush_color;

			std::unique_ptr<system::w_cpu> cpuInfo;
		};
	}
}

#endif