#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 i_pos;

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

void main() 
{
	vec4 _pos = vec4(i_pos, 1.0);
	vec4 _world_pos = u0.world * _pos;
	mat4 _world_view = u0.view * u0.world;

	gl_Position = u0.projection * u0.view * _world_pos;
}
