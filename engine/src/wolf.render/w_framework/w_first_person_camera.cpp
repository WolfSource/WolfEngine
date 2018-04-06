#include "w_render_pch.h"
#include "w_first_person_camera.h"
#include <glm/gtx/rotate_vector.hpp>

using namespace wolf::framework;
using namespace wolf::content_pipeline;

w_first_person_camera::w_first_person_camera() :
    _rotation_speed(1.0f),
    _movement_speed(200.0f),
	_rotation(glm::vec2(0))
{
}

w_first_person_camera::~w_first_person_camera()
{

}

bool w_first_person_camera::update(_In_ const wolf::system::w_game_time& pGameTime, 
    _In_ const w_point_t& pScreenSize)
{
    const int _W_KEY_CODE = 87;
    const int _S_KEY_CODE = 83;
    const int _D_KEY_CODE = 68;
    const int _A_KEY_CODE = 65;
    const int _Q_KEY_CODE = 81;
    const int _Z_KEY_CODE = 90;

    bool _updated = false;
    glm::vec3 _move_vector;

    auto _result_of_keys = inputs_manager.is_keys_pressed({ _W_KEY_CODE , _S_KEY_CODE , _A_KEY_CODE , _D_KEY_CODE, _Q_KEY_CODE, _Z_KEY_CODE });
    if (_result_of_keys[0] || _result_of_keys[1] ||
        _result_of_keys[2] || _result_of_keys[3] ||
        _result_of_keys[4] || _result_of_keys[5])
    {
        float _move_speed_time = pGameTime.get_elapsed_seconds() * this->_movement_speed;

        //W = Front
        if (_result_of_keys[0])
        {
            _move_vector.x += _move_speed_time;
        }
        //S = Back
        else if (_result_of_keys[1])
        {
            _move_vector.x -= _move_speed_time;
        }
        
        //D = Right
        if (_result_of_keys[2])
        {
            _move_vector.z += _move_speed_time;
        }
        //A = Left
        else if (_result_of_keys[3])
        {
            _move_vector.z -= _move_speed_time;
        }

        //Q = Up
        if (_result_of_keys[4])
        {
            _move_vector.y += _move_speed_time;
        }
        //Z = Down
        else if (_result_of_keys[5])
        {
            _move_vector.y -= _move_speed_time;
        }

        _updated = true;
    }

    if (inputs_manager.mouse.left_button_pressed)
    {
        _updated = true;

        if (inputs_manager.mouse.pos_x - inputs_manager.mouse.last_pos_x > 1)
        {
            this->_rotation.x += glm::radians(this->_rotation_speed);
        }
        else if (inputs_manager.mouse.pos_x - inputs_manager.mouse.last_pos_x < -1)
        {
            this->_rotation.x -= glm::radians(this->_rotation_speed);
        }
        else if (inputs_manager.mouse.pos_y - inputs_manager.mouse.last_pos_y > 1)
        {
            this->_rotation.y -= glm::radians(this->_rotation_speed) / 2.0f;
        }
        else if (inputs_manager.mouse.pos_y - inputs_manager.mouse.last_pos_y < -1)
        {
            this->_rotation.y += glm::radians(this->_rotation_speed) / 2.0f;
        }
    }

    if (_updated)
    {
        auto _camera_rotation = 
            glm::rotate(this->_rotation.x, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(this->_rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
        auto _vector_transform = _camera_rotation * glm::vec4(_move_vector, 1.0);

        this->_translate[0] += _vector_transform[0];
        this->_translate[1] += _vector_transform[1];
        this->_translate[2] += _vector_transform[2];

        _vector_transform = _camera_rotation * glm::vec4(1, 0, 0, 0);
        auto _target = glm::vec3(this->_translate[0], this->_translate[1], this->_translate[2]) + 
            glm::vec3(_vector_transform.x, _vector_transform.y, _vector_transform.z);

        this->_interest[0] = _target[0];
        this->_interest[1] = _target[1];
        this->_interest[2] = _target[2];

        update_view();
        update_frustum();
    }

    return _updated;
}

void w_first_person_camera::focus(_In_ const wolf::system::w_bounding_sphere& pBoundingSphere, _In_ const float& pDistance)
{
	set_interest(
		pBoundingSphere.center[0],
		pBoundingSphere.center[1],
		pBoundingSphere.center[2]);

	auto _radius = pBoundingSphere.radius * pDistance;
	set_translate(
		pBoundingSphere.center[0],
		pBoundingSphere.center[1] + _radius,
		pBoundingSphere.center[2] + _radius);

	update_view();
	update_frustum();
}