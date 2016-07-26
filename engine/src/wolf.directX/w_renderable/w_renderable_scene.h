/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_renderable_scene.h
	Description		 : Render c_scene using directX
	Comment          :
*/

#ifndef __W_RENDERABLE_SCENE_H__
#define __W_RENDERABLE_SCENE_H__

#include <w_graphics_device_manager.h>
#include <w_scene.h>
#include "w_renderable_model.h"

namespace wolf
{
	namespace graphics
	{
		namespace directX
		{
			class w_renderable_scene : public wolf::system::w_object
			{
			public:
				DX_EXP w_renderable_scene(wolf::content_pipeline::w_scene* pScene);
				DX_EXP void load(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);

				DX_EXP void render(const wolf::system::w_game_time& pGameTime);
				DX_EXP ULONG release();

#pragma region Getters

				DX_EXP void get_first_or_default_camera(wolf::content_pipeline::w_camera** pCamera);

#pragma endregion

#pragma region Setters

				DX_EXP void set_view_projection(DirectX::XMMATRIX pValue);

#pragma endregion

			private:
				typedef	 wolf::system::w_object				_super;

				size_t										_model_counts;
				std::vector<w_renderable_model*>			_models;
				wolf::content_pipeline::w_scene*			_scene;
			};
		}
	}
}

#endif