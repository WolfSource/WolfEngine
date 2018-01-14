#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;

layout(set = 0, binding = 0) uniform U0
{
	mat4	wvp;
} u0;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec4 o_color;

void main() 
{
    gl_Position = vec4(i_position, 1.0);
    o_color = i_color;
}
