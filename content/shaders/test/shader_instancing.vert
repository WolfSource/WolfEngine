#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set=0, binding=1) uniform u_buffer
{
    mat4 wvp;
} U0;

// vertex input attributes
layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

//vertex instance attributes
layout (location = 2) in vec3 i_instance_pos;
layout (location = 3) in vec3 i_instance_rot;
layout (location = 4) in float i_instance_scale;
layout (location = 5) in int i_instance_uv_index;


layout(location = 0) out vec2 o_uv;

void main() 
{
    if (i_instance_scale == 0)
    {
        //is ref model
         gl_Position = U0.wvp  * vec4(i_position, 1);
    }
    else
    {
        //is instance
	mat3 mx, my, mz;
	
	float s = sin(i_instance_rot.x);
	float c = cos(i_instance_rot.x);
    float _s = -1.0 * s;
    float _c = -1.0 * c;
    
	mx[0] = vec3(1.0, 0.0, 0.0);
	mx[1] = vec3(0.0, c, _s);
	mx[2] = vec3(0.0, s, c);
    
    s = sin(i_instance_rot.y);
    c = cos(i_instance_rot.y);
    _s = -1.0 * s;
    _c = -1.0 * c;
    
    my[0] = vec3(c, 0.0, s);
    my[1] = vec3(0.0, 1.0, 0.0);
    my[2] = vec3(_s, 0.0, c);
    
    s = sin(i_instance_rot.z);
    c = cos(i_instance_rot.z);
    _s = -1.0 * s;
    _c = -1.0 * c;
    
    mz[0] = vec3(c, _s, 0.0);
    mz[1] = vec3(s, c, 0.0);
    mz[2] = vec3(0.0, 0.0, 1.0);

    mat3 _rot_mat = mx * my * mz;
    
    vec4 _loc_pos = vec4(i_position * _rot_mat, 1.0);
    vec4 _pos = vec4((_loc_pos.xyz * 0 /*i_instance_scale*/) + i_instance_pos, 1.0);
    
    gl_Position = U0.wvp  * _pos;
    }
    o_uv = i_uv;//, i_instance_uv_index);
}
