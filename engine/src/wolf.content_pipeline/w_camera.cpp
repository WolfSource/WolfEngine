#include "w_cpipeline_pch.h"
#include "w_camera.h"

using namespace wolf::content_pipeline;

w_camera::w_camera() : 
    _aspect_ratio(1.777f), 
    _near_plane(0.01f), 
    _far_plane(1000.0f),
    _field_of_view(70.0f)
{
    this->_translate[0] = 0; this->_translate[1] = 5; this->_translate[2] = 100.0f;
    this->_interest[0] = 0; this->_interest[1] = 0; this->_interest[2] = 0;
    this->_up[0] = 0; this->_up[1] = 1; this->_up[2] = 0;
    
	update_view();
	update_projection();
}

w_camera::~w_camera()
{

}

void w_camera::update_view()
{
    this->_view = glm::lookAtRH(
            glm::vec3(this->_translate[0],
                this->_translate[1],
                this->_translate[2]),
            glm::vec3(this->_interest[0],
                this->_interest[1],
                this->_interest[2]),
            glm::vec3(this->_up[0],
                this->_up[1],
                this->_up[2]));
}

void w_camera::update_projection()
{
	auto _pi = glm::pi<float>();
	float _fov_angle_y = this->_field_of_view * _pi / 180.0f;

	if (this->_aspect_ratio < 1.0f)
	{
		// portrait or snap view
        this->_up[0] = 1.0f; this->_up[1] = 0.0f; this->_up[2] = 0.0f;
		_fov_angle_y = 120.0f * _pi / 180.0f;
	}
	else
	{
		// landscape view
        this->_up[0] = 0.0f; this->_up[1] = 1.0f; this->_up[2] = 0.0f;
    }

   this->_projection = glm::perspectiveRH(_fov_angle_y, this->_aspect_ratio, this->_near_plane, this->_far_plane);
}

#pragma region Setters

void w_camera::set_name(_In_z_ const std::string& pValue)
{
	this->_name = pValue;
}

void w_camera::set_camera_target_name(_In_z_ const std::string& pValue)
{
    this->_camera_target_name = pValue;
}

void w_camera::set_field_of_view(_In_ const float pValue)
{
    this->_field_of_view = pValue;
}

void w_camera::set_near_plan(_In_ const float pValue)
{
    this->_near_plane = pValue;
}

void w_camera::set_far_plan(_In_ const float pValue)
{
    this->_far_plane = pValue;
}

void w_camera::set_aspect_ratio(float pValue)
{
	this->_aspect_ratio = pValue;
}

void w_camera::set_translate(float pX, float pY, float pZ)
{
	this->_translate[0] = pX; this->_translate[1] = pY; this->_translate[2] = pZ;
}

void w_camera::set_translate(glm::vec3 pPosition)
{
	this->_translate[0] = pPosition.x;
    this->_translate[1] = pPosition.y;
    this->_translate[2] = pPosition.z;
}

void w_camera::set_interest(float pX, float pY, float pZ)
{
	this->_interest[0] = pX; 
    this->_interest[1] = pY; 
    this->_interest[2] = pZ;
}

void w_camera::set_interest(glm::vec3 pInterest)
{
	this->_interest[0] = pInterest.x;
    this->_interest[1] = pInterest.y;
    this->_interest[2] = pInterest.z;
}

#pragma endregion
