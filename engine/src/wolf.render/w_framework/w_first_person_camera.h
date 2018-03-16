/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_free_camera.h
	Description		 : The free camera
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_FIRST_PERSON_CAMERA_H__
#define __W_FIRST_PERSON_CAMERA_H__

#include "w_cpipeline_export.h"
#include <w_game_time.h>
#include <w_point.h>
#include <collada/c_camera.h>
#include <glm/vec2.hpp>

namespace wolf
{
	namespace framework
	{
		class w_first_person_camera : public content_pipeline::c_camera
		{
		public:
			WCP_EXP w_first_person_camera();
			WCP_EXP virtual ~w_first_person_camera();

            //returns true if camera updated
            WCP_EXP bool update(_In_ const wolf::system::w_game_time& pGameTime,
                _In_ const w_point_t& pScreenSize);

		private:
            float           _rotation_speed;
            float           _movement_speed;
            glm::vec2       _rotation;

		};
	}
}

#endif
