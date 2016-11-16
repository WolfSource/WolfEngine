/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_image.h
	Description		 : The static control which shows just image.

	Comment          :
*/

#ifndef __W_IMAGE__
#define __W_IMAGE__

#include "w_control.h"
#include "w_graphics/w_sprite_batch.h"
#include "w_graphics/w_geometries/w_mesh.h"

namespace wolf
{
	namespace gui
	{
		class w_image : public w_control
		{
		public:
			DX_EXP w_image(_In_opt_ w_widget* pParent);
			DX_EXP ~w_image();

			virtual HRESULT on_initialize(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			HRESULT create_or_update_vertex_buffer(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ bool pUpdate);
			virtual void render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual ULONG release() override;

#pragma region Getter

			DX_EXP const wchar_t*					get_path() const													{ return this->path.c_str(); }
			DX_EXP float							get_scale_x() const													{ return this->scale.x; }
			DX_EXP float							get_scale_y() const													{ return this->scale.y; }
			DX_EXP float							get_rotation_x() const												{ return this->rotation.x; }
			DX_EXP float							get_rotation_y() const												{ return this->rotation.y; }

#pragma endregion

#pragma region Setter

			DX_EXP HRESULT							set_path(_In_z_ const std::wstring& pValue);
			DX_EXP void								set_scale(_In_ const float pX, _In_ const float pY)					{ this->scale.x = pX; this->scale.y = pY;}
			DX_EXP void								set_rotation(_In_ const float pX, _In_ const float pY)				{ this->rotation.x = pX; this->rotation.y = pY;}

#pragma endregion

		protected:
			std::wstring							path;
			DirectX::XMFLOAT2						scale;
			DirectX::XMFLOAT2						rotation;

		private:
			typedef w_control _super;
			wolf::graphics::w_mesh*					_mesh;
			ID3D11Device1*							_device;
		};
	}
}

#endif
