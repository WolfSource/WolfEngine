#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec2 i_uv;

layout(location = 0) out vec4 o_color;
layout(location = 1) out vec2 o_uv;

void main() 
{
    gl_Position = vec4(i_position, 1);
    o_color = i_color;
    o_uv = i_uv;
}
