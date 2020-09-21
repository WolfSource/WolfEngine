#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding=0) uniform UBO
{
    mat4 projection_view;
	mat4 model;
} ubo;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

layout(location = 0) out vec2 o_uv;

void main() 
{
    gl_Position = ubo.projection_view * ubo.model * vec4(i_position, 1);
    o_uv = i_uv;
}
