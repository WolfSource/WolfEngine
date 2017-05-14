#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set=0, binding=1) uniform u_buffer
{
    mat4 projection_view;
	mat4 model;
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

//forward declaration 
mat3 rotate_over_axis(float pAngle, vec3 pAxis);

void main() 
{
    if (i_instance_scale == 0)
    {
        //is ref model
         gl_Position = U0.projection_view * U0.model * vec4(i_position, 1);
    }
    else
    {
        //is instance
        mat3 rx = rotate_over_axis(i_instance_rot.x, vec3( -1.0, 0.0, 0.0));
		mat3 ry = rotate_over_axis(i_instance_rot.y, vec3( 0.0, 1.0, 0.0));
		mat3 rz = rotate_over_axis(i_instance_rot.z, vec3( 0.0, 0.0, 1.0));

        mat3 _rot = rx * ry * rz;
		mat4 _model_mat = mat4( _rot[0][0]       , _rot[0][1]		, _rot[0][2]		, 0.0,
								_rot[1][0]       , _rot[1][1]		, _rot[1][2]		, 0.0,
								_rot[2][0]       , _rot[2][1]		, _rot[2][2]		, 0.0,
								i_instance_pos.x , i_instance_pos.y , i_instance_pos.z  , 1.0);

        gl_Position =  U0.projection_view * _model_mat * vec4(i_position, 1.0);
    }
    o_uv = i_uv;//, i_instance_uv_index);
}

//forward declaration 
mat3 rotate_over_axis(float pAngle, vec3 pAxis)
{
	const float a = pAngle;
	const float c = cos(a);
	const float s = sin(a);

	vec3 axis = normalize(pAxis);
	vec3 temp = (1 - c) * axis;

	mat3 rotate;
	rotate[0][0] = c + temp[0] * axis[0];
	rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	rotate[1][1] = c + temp[1] * axis[1];
	rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	rotate[2][2] = c + temp[2] * axis[2];

	return rotate;
}