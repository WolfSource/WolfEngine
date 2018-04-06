#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec3 i_norm;
layout(location = 2) in vec2 i_uv;

layout(binding = 0) uniform U0
{
	mat4 world;
	mat4 view;
	mat4 projection;
} u0;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout (location = 0) out vec3 o_norm;
layout (location = 1) out vec2 o_uv;
layout(location = 2) out vec3 o_color;

void main() 
{
	vec4 _pos = vec4(i_pos, 1.0);
	vec4 _world_pos = u0.world * _pos;
	mat4 _world_view = u0.view * u0.world;

	gl_Position = u0.projection * u0.view * _world_pos;
	o_norm =  normalize( ( vec4(i_norm, 0.0)  * _world_view ).xyz );
	o_uv = i_uv;

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
