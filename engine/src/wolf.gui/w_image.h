/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_image.h
	Description		 : The user control which shows image file.
	Comment          :
*/

#ifndef __W_IMAGE__
#define __W_IMAGE__

#include "w_control.h"

namespace wolf
{
	namespace gui
	{
		class w_image : public w_control
		{
		public:
			WGUI_EXP w_image(_In_opt_ w_widget* pParent);
			WGUI_EXP ~w_image();

			virtual void render_2d(_In_ const float& pElapsedTime, _In_ std::vector<w_gui_vertex_2d>& pVertexDeclarations) override;
			virtual ULONG release() override;

#pragma region Getter

			WGUI_EXP const wchar_t*					get_path() const													{ return this->path.c_str(); }
			WGUI_EXP const bool						get_is_absolute_path() const										{ return this->is_absolute_path; }
			WGUI_EXP float							get_rotation_x() const												{ return this->rotation.x; }
			WGUI_EXP float							get_rotation_y() const												{ return this->rotation.y; }
            WGUI_EXP float							get_opacity() const                                                 { return this->opacity; }
			WGUI_EXP glm::vec2						get_uv_scale() const                                                { return this->uv_scale; }
			WGUI_EXP bool							get_use_default_size() const										{ return this->use_default_size; }

#pragma endregion

#pragma region Setter

            WGUI_EXP void						    set_path(
                _In_z_ const std::wstring& pPath,
                _In_ const bool pIsAbsolutePath)                                                                        { this->path = pPath; this->is_absolute_path = pIsAbsolutePath; }
			WGUI_EXP void							set_rotation(_In_ const glm::vec2& pRotation)				        { this->rotation = pRotation;}
            WGUI_EXP void							set_opacity(_In_ const float& pValue)                               { this->opacity = pValue; }
            WGUI_EXP void							set_uv_scale(_In_ const glm::vec2& pValue)                          { this->uv_scale = pValue; }
			WGUI_EXP void							set_use_default_size(_In_ const bool pValue)						{ this->use_default_size = pValue; }

#pragma endregion

		protected:
			std::wstring							path;
			bool									is_absolute_path;
			bool									use_default_size;
            glm::vec2						        rotation;
            glm::vec2						        uv_scale;
            float                                   opacity;
            w_color                                 top_left;
            w_color                                 down_left;
            w_color                                 top_right;
            w_color                                 down_right;

		private:
			typedef w_control						_super;
		};
	}
}

#endif
