/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : W_DebugState.h
	Description		 : The change color of debug
	Comment          :
*/

#pragma once

#include <d2d1_1.h>
#include <W_GraphicsDeviceManager.h>

namespace Wolf
{
	namespace Graphics
	{
		class W_Debugger : public System::W_Object
		{
		public:
			//Constructor of W_Debugger
			W_Debugger(const std::shared_ptr<W_GraphicsDevice>& gDevice);
			//Destructor of W_Debugger
			virtual ~W_Debugger();

			//Release all resources
			ULONG Release() override;

#pragma region Getters
			//Get color state - Normal = Lime, Warning = Yellow, Critical = Red
			ID2D1SolidColorBrush* GetColorState();
#pragma endregion

		protected:
			enum State
			{
				Normal, Warning, Error
			};
			State state;

			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> RedSolidBrushColor;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> YellowSolidBrushColor;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> LimeSolidBrushColor;
		};
	}
}