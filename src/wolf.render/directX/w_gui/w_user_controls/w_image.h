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
#include "w_graphics/w_geometries/w_quad.h"

namespace wolf
{
	namespace gui
	{
		class w_image : public w_control, public wolf::graphics::w_quad<>
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
			DX_EXP const bool						get_is_absolute_path() const										{ return this->is_absolute_path; }
			DX_EXP float							get_rotation_x() const												{ return this->rotation.x; }
			DX_EXP float							get_rotation_y() const												{ return this->rotation.y; }
			DX_EXP float							get_opacity() const;
			DX_EXP glm::vec2						get_uv_scale() const;
			DX_EXP bool								get_use_default_size() const										{ return this->use_default_size; }

#pragma endregion

#pragma region Setter

			DX_EXP HRESULT							set_path(_In_z_ const std::wstring& pValue, _In_ const bool pIsAbsolutePath);
			DX_EXP void								set_rotation(_In_ const float pX, _In_ const float pY)				{ this->rotation.x = pX; this->rotation.y = pY;}
			DX_EXP void								set_opacity(_In_ const float pValue);
			DX_EXP void								set_uv_scale(_In_ const glm::vec2 pValue);
			DX_EXP void								set_use_default_size(_In_ const bool pValue)						{ this->use_default_size = pValue; }

#pragma endregion

		protected:
			std::wstring							path;
			bool									is_absolute_path;
			bool									use_default_size;
			DirectX::XMFLOAT2						rotation;

		private:
			typedef w_control						_super;
			typedef wolf::graphics::w_quad<>		_quad;

			ID3D11Device1*							_device;
		};
	}
}

#endif
