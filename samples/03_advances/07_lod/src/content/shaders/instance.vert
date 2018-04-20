#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec3 i_norm;
layout(location = 2) in vec2 i_uv;

layout (location = 3) in vec3	i_ins_pos;
layout (location = 4) in vec3	i_ins_rot;

layout(binding = 0) uniform U0
{
	mat4	view;
	mat4	projection;
	vec4	camera_pos;//w is padding
} u0;

layout(binding = 1) uniform U1
{
	uint	texture_max_mip_maps_max_level;
	float	bounding_sphere_radius;
	float	padding_0;
	float	padding_1;
} u1;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout (location = 0) out vec3 o_norm;
layout (location = 1) out vec2 o_uv;
layout(location = 2) out flat uint o_texture_mip_map_level;
layout(location = 3) out vec3 o_color;

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

void main() 
{
	mat3 rx = rotate_over_axis(i_ins_rot.x, vec3( 1.0, 0.0, 0.0));
	mat3 ry = rotate_over_axis(i_ins_rot.y, vec3( 0.0, 1.0, 0.0));
	mat3 rz = rotate_over_axis(i_ins_rot.z, vec3( 0.0, 0.0, 1.0));

	const float i_ins_scale = 1.0;
	mat3 _rot = rx * ry * rz;
	mat4 _world = mat4( _rot[0][0]  * i_ins_scale 		, _rot[0][1]					, _rot[0][2]					, 0.0,
						_rot[1][0]						, _rot[1][1] * i_ins_scale		, _rot[1][2]					, 0.0,
						_rot[2][0]						, _rot[2][1]					, _rot[2][2] * i_ins_scale		, 0.0,
						i_ins_pos.x						, i_ins_pos.y					, i_ins_pos.z					, 1.0);

	vec4 _pos = vec4(i_pos, 1.0);
	vec4 _world_pos = _world * _pos;
	mat4 _world_view = u0.view * _world;

	gl_Position = u0.projection * u0.view * _world_pos;
	o_norm =  normalize( ( vec4(i_norm, 0.0)  * _world_view ).xyz );
	o_uv = i_uv;

	//set texture mip map based on position
	o_texture_mip_map_level = u1.texture_max_mip_maps_max_level - 1;
	float _distance_from_cam = distance(u0.camera_pos, vec4(i_ins_pos, 1.0));
	float _bs = u1.bounding_sphere_radius * 10; 
	for(int i = 0; i < u1.texture_max_mip_maps_max_level; ++i)
	{
		if (_distance_from_cam <= _bs * (i + 1))
		{
			o_texture_mip_map_level = i;
			break;
		}
	}

	if (gl_InstanceIndex == 0)
	{
		//this is ref model
		o_color = vec3(1.0, 0.0, 0.0);
	}
	else
	{
		//this is instance model
		o_color = vec3(0.0, 1.0, 0.0);
	}
}
