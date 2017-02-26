#include "w_cpipeline_pch.h"
#include "w_camera.h"

using namespace wolf::content_pipeline;

w_camera::w_camera() : aspect_ratio(1.777f), near_plane(0.01f), far_plane(1000.0f),
transform(0, 2, 20.0f), interest(0, 0, 0), up(0, 1, 0), field_of_view(70.0f)
{
	update_view();
	update_projection();
}

w_camera::~w_camera()
{

}

void w_camera::update_view()
{
	this->view = glm::lookAtRH(this->transform, this->interest, this->up);
}

void w_camera::update_projection()
{
	auto _pi = glm::pi<float>();
	float _fov_angle_y = this->field_of_view * _pi / 180.0f;

	if (this->aspect_ratio < 1.0f)
	{
		// portrait or snap view
		this->up.x = 1.0f; this->up.y = 0.0f; this->up.z = 0.0f;
		_fov_angle_y = 120.0f * _pi / 180.0f;
	}
	else
	{
		// landscape view
		this->up.x = 0.0f; this->up.y = 1.0f; this->up.z = 0.0f;
	}

	this->projection = glm::perspectiveRH(_fov_angle_y, this->aspect_ratio, this->near_plane, this->far_plane);
}

#pragma region Setters

void w_camera::set_name(const std::string& pName)
{
	this->name = pName;
}

void w_camera::set_aspect_ratio(float pValue)
{
	this->aspect_ratio = pValue;
}

void w_camera::set_transform(float pX, float pY, float pZ)
{
	this->transform.x = pX; this->transform.y = pY; this->transform.z = pZ;
}

void w_camera::set_transform(glm::vec3 pPosition)
{
	this->transform = pPosition;
}

void w_camera::set_interest(float pX, float pY, float pZ)
{
	this->interest.x = pX; this->interest.y = pY; this->interest.z = pZ;
}

void w_camera::set_interest(glm::vec3 pInterest)
{
	this->interest = pInterest;
}

#pragma endregion