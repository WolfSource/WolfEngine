#include "w_cpipeline_pch.h"
#include "w_camera.h"

using namespace wolf::content_pipeline;

w_camera::w_camera() :
    _type(w_camera_type::target),
    _aspect_ratio(1.777f),
    _near_plane(0.01f),
    _far_plane(1000.0f),
    _field_of_view(45.0f),
    _z_up(true)
{
    this->_position[0] = 0; this->_position[1] = 7; this->_position[2] = -27.0f;
    this->_look_at[0] = 0; this->_look_at[1] = 0; this->_look_at[2] = 0;
    this->_up[0] = 0.0f; this->_up[1] = -1.0f; this->_up[2] = 0.0f;
}

w_camera::~w_camera()
{

}

void w_camera::update_view()
{
    const glm::vec3 _pos = glm::vec3(this->_position[0], this->_position[1], this->_position[2]);
    const glm::vec3 _look = glm::vec3(this->_look_at[0], this->_look_at[1], this->_look_at[2]);
    const glm::vec3 _u = glm::vec3(this->_up[0], this->_up[1], this->_up[2]);

    this->_view = glm::lookAtRH(_pos, _look, _u);
}

void w_camera::update_projection()
{
	auto _pi = glm::pi<float>();
    float _fov_angle_y = this->_field_of_view * _pi / 180.0f;

	if (this->_aspect_ratio < 1.0f)
	{
		// portrait or snap view
        this->_up[0] = -1.0f; this->_up[1] = 0.0f; this->_up[2] = 0.0f;
		_fov_angle_y = 120.0f * _pi / 180.0f;
	}
	else
	{
		// landscape view
        this->_up[0] = 0.0f; this->_up[1] = -1.0f; this->_up[2] = 0.0f;
    }

   this->_projection = glm::perspectiveRH(_fov_angle_y, this->_aspect_ratio, this->_near_plane, this->_far_plane);
}

void w_camera::update_frustum()
{
    auto _matrix = this->_projection * this->_view;
	this->_frustum.update(_matrix);
}

#pragma region Getters

mat4x4_p w_camera::get_projection_view() const
{
    return this->_projection * this->_view;
}

const glm::vec3 w_camera::get_position() const
{
	return glm::vec3(this->_position[0], this->_position[1], this->_position[2]);
}

const glm::vec3 w_camera::get_look_at() const
{ 
	return glm::vec3(this->_look_at[0], this->_look_at[1], this->_look_at[2]);
}

#pragma endregion

#pragma region Setters

void w_camera::set_name(_In_z_ const std::string& pValue)
{
	this->_name = pValue;
}

void w_camera::set_camera_target_name(_In_z_ const std::string& pValue)
{
    this->_camera_target_name = pValue;
}

void w_camera::set_field_of_view(_In_ const float& pValue)
{
    this->_field_of_view = pValue;
}

void w_camera::set_near_plan(_In_ const float& pValue)
{
    this->_near_plane = pValue;
}

void w_camera::set_far_plan(_In_ const float& pValue)
{
    this->_far_plane = pValue;
}

void w_camera::set_aspect_ratio(_In_ const float& pValue)
{
	this->_aspect_ratio = pValue;
}

void w_camera::set_position(_In_ const float& pX, _In_ const float& pY, _In_ const float& pZ)
{
	this->_position[0] = pX; this->_position[1] = pY; this->_position[2] = pZ;
}

void w_camera::set_position(_In_ const glm::vec3& pPosition)
{
	this->_position[0] = pPosition.x; this->_position[1] = pPosition.y; this->_position[2] = pPosition.z;
}

void w_camera::set_look_at(_In_ const float& pX, _In_ const float& pY, _In_ const float& pZ)
{
	this->_look_at[0] = pX; this->_look_at[1] = pY; this->_look_at[2] = pZ;
}

void w_camera::set_look_at(_In_ const glm::vec3& pInterest)
{
	this->_look_at[0] = pInterest.x; this->_look_at[1] = pInterest.y; this->_look_at[2] = pInterest.z;
}

void w_camera::set_up_vector(_In_ const float& pX, _In_ const float& pY, _In_ const float& pZ)
{
    this->_up[0] = pX; this->_up[1] = pY; this->_up[2] = pZ;
}

void w_camera::set_up_vector(_In_ const glm::vec3& pUp)
{
     this->_up[0] = pUp.x; this->_up[1] = pUp.y; this->_up[2] = pUp.z;
}

#pragma endregion
