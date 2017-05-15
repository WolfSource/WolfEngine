#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding=1) uniform sampler2D u_samplers;
layout (binding=2) uniform U2
{
	vec4 color;
} u;

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main()
{
    vec4 _color = u.color * texture( u_samplers, i_uv );
	if(_color.a == 0)
	{
		discard;
	}
	o_color = _color;
}
