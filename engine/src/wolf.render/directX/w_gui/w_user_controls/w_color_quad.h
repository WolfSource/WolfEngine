/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_color_quad.h
	Description		 : The static control which shows a colored quad.
	Comment          :
*/

#ifndef __W_COLOR_QUAD__
#define __W_COLOR_QUAD__

#include "w_control.h"
#include "w_graphics/w_geometries/w_quad.h"

namespace wolf
{
	namespace gui
	{
		class w_color_quad : public w_control, public wolf::graphics::w_quad<wolf::graphics::quad_color_const_buffer>
		{
		public:
			DX_EXP w_color_quad(_In_opt_ w_widget* pParent);
			DX_EXP ~w_color_quad();

			virtual HRESULT on_initialize(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			HRESULT create_or_update_vertex_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ bool pUpdate);
			virtual void render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual ULONG release() override;

#pragma region Getter

			DX_EXP const float						get_rotation_x() const												{ return this->rotation.x; }
			DX_EXP const float						get_rotation_y() const												{ return this->rotation.y; }
			DX_EXP const w_color					get_color() const;
			
#pragma endregion

#pragma region Setter

			DX_EXP void								set_rotation(_In_ const float pX, _In_ const float pY)				{ this->rotation.x = pX; this->rotation.y = pY;}
			DX_EXP void								set_color(_In_ const w_color pValue);

#pragma endregion

		protected:
			std::wstring							path;
			DirectX::XMFLOAT2						rotation;
			w_color									color;

		private:
			typedef w_control															_super;
			typedef wolf::graphics::w_quad<wolf::graphics::quad_color_const_buffer>		_quad;

			ID3D11Device1*																_device;
		};
	}
}

#endif
