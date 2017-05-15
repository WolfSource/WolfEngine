#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding=0) uniform u_buffer
{
    mat4 projection_view;
	mat4 model;
} U0;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

layout(location = 0) out vec2 o_uv;

void main() 
{
    gl_Position = U0.projection_view * U0.model * vec4(i_position, 1);
    o_uv = i_uv;
}
