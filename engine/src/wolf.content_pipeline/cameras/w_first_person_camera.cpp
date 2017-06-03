#include "w_cpipeline_pch.h"
#include "w_first_person_camera.h"

using namespace wolf::content_pipeline;

w_first_person_camera::w_first_person_camera() :
    _rotation_speed(0.4f),
    _movement_speed(100.0f)
{
    this->_rotation[0] = 0;
    this->_rotation[1] = 0;
}

w_first_person_camera::~w_first_person_camera()
{

}

static glm::vec3 transform(glm::vec3 position, glm::mat4 matrix)
{
    glm::vec3 vector;
    float num3 = (((position.x * matrix[0][0]) + (position.y * matrix[1][0])) + (position.z * matrix[2][0])) + matrix[3][0];
    float num2 = (((position.x * matrix[0][1]) + (position.y * matrix[1][1])) + (position.z * matrix[2][3])) + matrix[3][1];
    float num = (((position.x * matrix[0][2]) + (position.y * matrix[1][2])) + (position.z * matrix[2][2])) + matrix[3][2];
    vector.x = num3;
    vector.y = num2;
    vector.z = num;
    return vector;
}

void w_first_person_camera::update(_In_ const wolf::system::w_game_time& pGameTime, 
    _In_ const w_point_t& pScreenSize)
{
    const int _W_KEY_CODE = 87;
    const int _S_KEY_CODE = 83;
    const int _D_KEY_CODE = 68;
    const int _A_KEY_CODE = 65;

    bool _update_view = false;
    glm::vec3 _move_vector;

    auto _result_of_keys = inputs_manager.is_keys_pressed({ _W_KEY_CODE , _S_KEY_CODE , _A_KEY_CODE , _D_KEY_CODE });
    if (_result_of_keys[0] || _result_of_keys[1] ||
        _result_of_keys[2] || _result_of_keys[3])
    {
        float _move_speed_time = pGameTime.get_elapsed_seconds() * this->_movement_speed;

        //Up
        if (_result_of_keys[0])
        {
            _move_vector.x += _move_speed_time;
        }
        //Down
        else if (_result_of_keys[1])
        {
            _move_vector.x -= _move_speed_time;
        }
        //Left
        else if (_result_of_keys[2])
        {
            _move_vector.y += _move_speed_time;
        }
        //Right
        else if (_result_of_keys[3])
        {
            _move_vector.y -= _move_speed_time;
        }

        _update_view = true;
    }

    if (inputs_manager.mouse.left_button_pressed)
    {
        _update_view = true;

        if (inputs_manager.mouse.pos_x - inputs_manager.mouse.last_pos_x > 1)
        {
            this->_rotation[0] -= glm::radians(this->_rotation_speed);
        }
        else if (inputs_manager.mouse.pos_x - inputs_manager.mouse.last_pos_x < -1)
        {
            this->_rotation[0] += glm::radians(this->_rotation_speed);
        }

        else if (inputs_manager.mouse.pos_y - inputs_manager.mouse.last_pos_y > 1)
        {
           // this->_rotation[1] -= glm::radians(this->_rotation_speed);
        }
        else if (inputs_manager.mouse.pos_x - inputs_manager.mouse.last_pos_x < -1)
        {
            //this->_rotation[1] += glm::radians(this->_rotation_speed);
        }

        auto _pi = glm::pi<float>();

        //if (this->_rotation[0] > 1.4) this->_rotation[0] = 1.4f;
        //if (this->_rotation[0] < -1.4) this->_rotation[0] = -1.4f;
        //if (this->_rotation[1] >  _pi) this->_rotation[1] -= 2 * _pi;
        //if (this->_rotation[1] < -_pi) this->_rotation[1] += 2 * _pi;
    }

    if (_update_view)
    {
        auto _camera_rotation = glm::rotate(this->_rotation[0], glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(this->_rotation[1], glm::vec3(-1.0f, 0.0f, 0.0f));
        auto _position = transform(_move_vector, _camera_rotation);
        
        this->_translate[0] += _position[0];
        this->_translate[1] += _position[1];
        this->_translate[2] += _position[2];

        auto __up = transform(glm::vec3(this->_up[0], this->_up[1], this->_up[2]), _camera_rotation);
        __up = glm::normalize(__up);

        this->_up[0] = __up.x;
        this->_up[1] = __up.y;
        this->_up[2] = __up.z;

        auto _target = glm::vec3(this->_translate[0], this->_translate[1], this->_translate[2]) +
            transform(glm::vec3(1,0,0), _camera_rotation);

        this->_interest[0] = _target[0];
        this->_interest[1] = _target[1];
        this->_interest[2] = _target[2];

        update_view();
    }
}
