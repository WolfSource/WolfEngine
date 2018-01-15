#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform U1
{
	vec4	color;
} u1;

layout(location = 0) out vec4 o_color;

void main() 
{
	o_color = u1.color;
	if (o_color.a == 0) discard;	
}
