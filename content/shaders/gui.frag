#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding=0) uniform sampler2D u_samplers;

layout(location = 0) in vec4 i_color;
layout(location = 1) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main()
{
    vec4 _color = i_color * texture( u_samplers, i_uv );
	if(_color.a == 0)
	{
		discard;
	}
	o_color = _color;
}
