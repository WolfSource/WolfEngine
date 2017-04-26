/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_camera.h
	Description		 : The base class for camera
	Comment          :
*/

#ifndef __W_CAMERA_H__
#define __W_CAMERA_H__

#include "w_cpipeline_export.h"
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace wolf
{
	namespace content_pipeline
	{
		typedef glm::tmat4x4<float, glm::precision::highp> mat4x4_p;

		class w_camera
		{
		public:
			WCP_EXP w_camera();
			WCP_EXP virtual ~w_camera();

			//Update the view matrix
			WCP_EXP void update_view();
			//Update the projection matrix
			WCP_EXP void update_projection();

#pragma region Getters

			WCP_EXP std::string get_name() const							{ return this->name; };
			//Get field of view
			WCP_EXP float get_field_of_view() const						{ return this->field_of_view; }
			//Get aspect ratio of the camera
			WCP_EXP float get_aspect_ratio() const						{ return this->aspect_ratio; }
			//Get near plane of the camera
			WCP_EXP float get_near_plane() const							{ return this->near_plane; }
			//Get far plane of the camera
			WCP_EXP float get_far_plane() const							{ return this->far_plane; }
			//Get view matrix of the camera
			WCP_EXP mat4x4_p get_view() const							{ return this->view; }
			//Get projection matrix of the camera
			WCP_EXP mat4x4_p get_projection() const						{ return this->projection; }
			//Get view * projection matrix of the camera
			WCP_EXP mat4x4_p get_view_projection() const					{ return this->projection * this->view; }
			//Get position of the camera
			WCP_EXP glm::vec3 get_transform() const						{ return this->transform; }
			//Get interest of the camera
			WCP_EXP glm::vec3 get_interest() const						{ return this->interest; }

#pragma region 

#pragma region Setters

			//Set name of camera
			WCP_EXP void set_name(const std::string& pName);
			//Set aspect ratio of the camera
			WCP_EXP void set_aspect_ratio(float pAspectRatio);
			//Set position of the camera
			WCP_EXP void set_transform(float pX, float pY, float pZ);
			//Set position of the camera
			WCP_EXP void set_transform(glm::vec3 pInterest);
			//Set camera interest
			WCP_EXP void set_interest(float pX, float pY, float pZ);
			//Set camera interest
			WCP_EXP void set_interest(glm::vec3 pInterest);

#pragma endregion

		protected:
			std::string		name;
			float			field_of_view;
			float			aspect_ratio;
			float			near_plane;
			float			far_plane;
			glm::vec3		up;
			glm::vec3		transform;
			glm::vec3		interest;
			mat4x4_p		view;
			mat4x4_p		projection;
		};
	}
}

#endif
