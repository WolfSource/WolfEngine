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

namespace wolf
{
	namespace gui
	{
		class w_image : public w_control
		{
		public:
			w_image(_In_opt_ w_widget* pParent);

			virtual void render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;

#pragma region Getter

			const wchar_t*		get_path() const													{ return this->_path.c_str(); }
			DirectX::XMFLOAT2	get_scale() const													{ return this->_scale; }
			float				get_rotation_angle() const											{ return this->_rotation_angle; }
			DirectX::XMFLOAT2	get_rotation_center() const											{ return this->_rotation_center; }
			DirectX::XMFLOAT2	get_translation() const												{ return this->_translation; }
			float				get_guasian_effect_value() const									{ return this->_guasian_effect_value; }

#pragma endregion

#pragma region Setter

			void				set_path(const std::wstring& pValue)								{ this->_path = pValue; }
			void				set_scale(DirectX::XMFLOAT2 pValue)									{ this->_scale = pValue; }
			void				set_rotation_angle(float pValue)									{ this->_rotation_angle = pValue; }
			void				set_rotation_center(DirectX::XMFLOAT2 pValue)						{ this->_rotation_center = pValue; }
			void				set_translation(DirectX::XMFLOAT2 pValue)							{ this->_translation = pValue; }
			void				set_guasian_effect_value(float pValue)								{ this->_guasian_effect_value = pValue; }

#pragma endregion

		protected:
			std::wstring _path;
			float _guasian_effect_value;
			DirectX::XMFLOAT2 _scale;
			float _rotation_angle;
			DirectX::XMFLOAT2 _rotation_center;
			DirectX::XMFLOAT2 _translation;
			D2D1_RECT_F _crop;

		private:
			typedef w_control _super;
		};
	}
}

#endif
