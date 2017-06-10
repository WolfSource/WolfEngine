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
    this->_translate[0] = 0; this->_translate[1] = 7; this->_translate[2] = 27.0f;
    this->_interest[0] = 0; this->_interest[1] = 0; this->_interest[2] = 0;
    this->_up[0] = 0; this->_up[1] = 1; this->_up[2] = 0;

    update_view();
    update_projection();
    update_frustum();
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

void w_camera::update_frustum()
{
    auto _matrix = this->_projection * this->_view;

    //Near
    this->_frustum_planes[0].x = _matrix[0][3] + _matrix[0][2];
    this->_frustum_planes[0].y = _matrix[1][3] + _matrix[1][2];
    this->_frustum_planes[0].z = _matrix[2][3] + _matrix[2][2];
    this->_frustum_planes[0].w = _matrix[3][3] + _matrix[3][2];
    this->_frustum_planes[0] = glm::normalize(this->_frustum_planes[0]);

    //Far
    this->_frustum_planes[1].x = _matrix[0][3] - _matrix[0][2];
    this->_frustum_planes[1].y = _matrix[1][3] - _matrix[1][2];
    this->_frustum_planes[1].z = _matrix[2][3] - _matrix[2][2];
    this->_frustum_planes[1].w = _matrix[3][3] - _matrix[3][2];
    this->_frustum_planes[1] = glm::normalize(this->_frustum_planes[1]);

    //Left
    this->_frustum_planes[2].x = _matrix[0][3] + _matrix[0][0];
    this->_frustum_planes[2].y = _matrix[1][3] + _matrix[1][0];
    this->_frustum_planes[2].z = _matrix[2][3] + _matrix[2][0];
    this->_frustum_planes[2].w = _matrix[3][3] + _matrix[3][0];
    this->_frustum_planes[2] = glm::normalize(this->_frustum_planes[2]);

    //Right
    this->_frustum_planes[3].x = _matrix[0][3] - _matrix[0][0];
    this->_frustum_planes[3].y = _matrix[1][3] - _matrix[1][0];
    this->_frustum_planes[3].z = _matrix[2][3] - _matrix[2][0];
    this->_frustum_planes[3].w = _matrix[3][3] - _matrix[3][0];
    this->_frustum_planes[3] = glm::normalize(this->_frustum_planes[3]);

    //Top
    this->_frustum_planes[4].x = _matrix[0][3] - _matrix[0][1];
    this->_frustum_planes[4].y = _matrix[1][3] - _matrix[1][1];
    this->_frustum_planes[4].z = _matrix[2][3] - _matrix[2][1];
    this->_frustum_planes[4].w = _matrix[3][3] - _matrix[3][1];
    this->_frustum_planes[4] = glm::normalize(this->_frustum_planes[4]);
    
    //Top
    this->_frustum_planes[5].x = _matrix[0][3] + _matrix[0][1];
    this->_frustum_planes[5].y = _matrix[1][3] + _matrix[1][1];
    this->_frustum_planes[5].z = _matrix[2][3] + _matrix[2][1];
    this->_frustum_planes[5].w = _matrix[3][3] + _matrix[3][1];
    this->_frustum_planes[5] = glm::normalize(this->_frustum_planes[5]);













    ////Left
    //this->_frustum_planes[_left].x = _matrix[0].w + _matrix[0].x;
    //if (this->_z_up)
    //{
    //    this->_frustum_planes[_left].z = _matrix[1].w + _matrix[1].x;
    //    this->_frustum_planes[_left].y = _matrix[2].w + _matrix[2].x;
    //}
    //else
    //{
    //    this->_frustum_planes[_left].y = _matrix[1].w + _matrix[1].x;
    //    this->_frustum_planes[_left].z = _matrix[2].w + _matrix[2].x;
    //}
    //this->_frustum_planes[_left].w = _matrix[3].w + _matrix[3].x;

    ////Right
    //this->_frustum_planes[_right].x = _matrix[0].w - _matrix[0].x;
    //if (this->_z_up)
    //{
    //    this->_frustum_planes[_right].z = _matrix[1].w - _matrix[1].x;
    //    this->_frustum_planes[_right].y = _matrix[2].w - _matrix[2].x;
    //}
    //else
    //{
    //    this->_frustum_planes[_right].y = _matrix[1].w - _matrix[1].x;
    //    this->_frustum_planes[_right].z = _matrix[2].w - _matrix[2].x;
    //}
    //this->_frustum_planes[_right].w = _matrix[3].w - _matrix[3].x;

    ////Top
    //this->_frustum_planes[_top].x = _matrix[0].w - _matrix[0].y;
    //if (this->_z_up)
    //{
    //    this->_frustum_planes[_top].z = _matrix[1].w - _matrix[1].y;
    //    this->_frustum_planes[_top].y = _matrix[2].w - _matrix[2].y;
    //}
    //else
    //{
    //    this->_frustum_planes[_top].y = _matrix[1].w - _matrix[1].y;
    //    this->_frustum_planes[_top].z = _matrix[2].w - _matrix[2].y;
    //}
    //this->_frustum_planes[_top].w = _matrix[3].w - _matrix[3].y;

    ////Bottom
    //this->_frustum_planes[_bottom].x = _matrix[0].w + _matrix[0].y;
    //if (this->_z_up)
    //{
    //    this->_frustum_planes[_bottom].z = _matrix[1].w + _matrix[1].y;
    //    this->_frustum_planes[_bottom].y = _matrix[2].w + _matrix[2].y;
    //}
    //else
    //{
    //    this->_frustum_planes[_bottom].y = _matrix[1].w + _matrix[1].y;
    //    this->_frustum_planes[_bottom].z = _matrix[2].w + _matrix[2].y;
    //}
    //this->_frustum_planes[_bottom].w = _matrix[3].w + _matrix[3].y;

    ////Back
    //this->_frustum_planes[_back].x = _matrix[0].w + _matrix[0].z;
    //if (this->_z_up)
    //{
    //    this->_frustum_planes[_back].z = _matrix[1].w + _matrix[1].z;
    //    this->_frustum_planes[_back].y = _matrix[2].w + _matrix[2].z;
    //}
    //else
    //{
    //    this->_frustum_planes[_back].y = _matrix[1].w + _matrix[1].z;
    //    this->_frustum_planes[_back].z = _matrix[2].w + _matrix[2].z;
    //}
    //this->_frustum_planes[_back].w = _matrix[3].w + _matrix[3].z;

    ////Front
    //this->_frustum_planes[_front].x = _matrix[0].w - _matrix[0].z;
    //if (this->_z_up)
    //{
    //    this->_frustum_planes[_front].z = _matrix[1].w - _matrix[1].z;
    //    this->_frustum_planes[_front].y = _matrix[2].w - _matrix[2].z;
    //}
    //else
    //{
    //    this->_frustum_planes[_front].y = _matrix[1].w - _matrix[1].z;
    //    this->_frustum_planes[_front].z = _matrix[2].w - _matrix[2].z;
    //}
    //this->_frustum_planes[_front].w = _matrix[3].w - _matrix[3].z;

    //for (size_t i = 0; i < this->_frustum_planes.size(); i++)
    //{
    //    float length = sqrtf(
    //        this->_frustum_planes[i].x * this->_frustum_planes[i].x +
    //        this->_frustum_planes[i].y * this->_frustum_planes[i].y +
    //        this->_frustum_planes[i].z * this->_frustum_planes[i].z);
    //    this->_frustum_planes[i] /= length;
    //}
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
