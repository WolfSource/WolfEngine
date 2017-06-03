/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_free_camera.h
	Description		 : The free camera
	Comment          :
*/

#ifndef __W_FIRST_PERSON_CAMERA_H__
#define __W_FIRST_PERSON_CAMERA_H__

#include "w_cpipeline_export.h"
#include "w_camera.h"
#include <w_game_time.h>
#include <w_point.h>

namespace wolf
{
	namespace content_pipeline
	{
		class w_first_person_camera : public w_camera
		{
		public:
			WCP_EXP w_first_person_camera();
			WCP_EXP virtual ~w_first_person_camera();

            WCP_EXP void update(_In_ const wolf::system::w_game_time& pGameTime,
                _In_ const w_point_t& pScreenSize);

#pragma region Getters

#pragma region 

#pragma region Setters

            
#pragma endregion

		private:
            float           _rotation_speed;
            float           _movement_speed;
            float           _rotation[2];

		};
	}
}

#endif
