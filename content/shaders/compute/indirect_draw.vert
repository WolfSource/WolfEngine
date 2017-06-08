#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//input vertex attributes
layout(location = 0) in vec3	i_pos;
layout(location = 1) in vec2	i_uv;

//input vertex instance attributes
layout (location = 2) in vec3	i_ins_pos;
layout (location = 3) in vec3	i_ins_rot;
layout (location = 4) in float	i_ins_scale;

layout (binding = 0) uniform UBO_In
{
	mat4 projection_view;
} i_ubo;

layout (binding = 0) uniform UBO 
{
	mat4 projection_view;
} ubo;

//Out
layout (location = 0) out vec2 o_uv;

void main() 
{
	vec4 _pos = vec4((i_pos.xyz * i_ins_scale) + i_ins_pos, 1.0);
	gl_Position = i_ubo.projection_view * _pos;

	o_uv = i_uv;
}
