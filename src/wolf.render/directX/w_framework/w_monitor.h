/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_monitoring.h
	Description		 : The Monitoring
	Comment          :
*/

#ifndef __W_MONITOR_H__
#define __W_MONITOR_H__

#include <d2d1_1.h>
#include <w_graphics_device_manager.h>
#include <w_graphics\w_sprite_batch.h>
#include <w_game_time.h>
#include <w_cpu.h>

namespace wolf
{
	namespace framework
	{
		class w_monitor : public system::w_object
		{
		public:
			DX_EXP w_monitor(_In_ const std::shared_ptr<graphics::w_graphics_device>& gDevice);
			DX_EXP virtual ~w_monitor();

			//Print information of monitoring on the screen
			DX_EXP void printf(_In_ graphics::w_sprite_batch* pSpriteBatch, _In_ const system::w_game_time& pGameTime, _In_ DirectX::XMFLOAT2* const pPosition);

			//Release all resources
			DX_EXP ULONG release() override;

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