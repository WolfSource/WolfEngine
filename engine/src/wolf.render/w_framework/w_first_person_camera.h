/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_free_camera.h
	Description		 : The free camera
	Comment          :
*/

#pragma once

#include "w_cpipeline_export.h"
#include <w_game_time.h>
#include <w_point.h>
#include <w_bounding.h>
#include <w_camera.h>
#include <glm/vec2.hpp>

namespace wolf::framework
{
	class w_first_person_camera : public content_pipeline::w_camera
	{
	public:
		WCP_EXP w_first_person_camera();
		WCP_EXP virtual ~w_first_person_camera();

		//returns true if camera updated
		WCP_EXP bool update(_In_ const wolf::system::w_game_time& pGameTime,
			_In_ const w_point_t& pScreenSize);

		//focus on a object
		WCP_EXP void focus(_In_ const system::w_bounding_sphere& pBoundingSphere, _In_ const float& pDistance = 3.0f);

#pragma region Getters

		float get_rotation_speed() const { return this->_rotation_speed; }
		float get_movement_speed() const { return this->_movement_speed; }

#pragma endregion

#pragma region Setters

		void set_rotation_speed(_In_ const float& pValue) { this->_rotation_speed = pValue; }
		void set_movement_speed(_In_ const float& pValue) { this->_movement_speed = pValue; }

#pragma endregion

	private:
		float           _rotation_speed;
		float           _movement_speed;
		glm::vec2       _rotation;

	};
}
