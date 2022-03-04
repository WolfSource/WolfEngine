#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) out vec4 o_color;

void main() 
{
	o_color = vec4(0.333, 0.109, 0.694, 1.0);
}
