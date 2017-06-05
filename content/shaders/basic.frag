#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding=1) uniform sampler2D tex_sampler;

//must be push color
layout (binding=2) uniform UBO
{
	vec4 color;
} ubo;

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main()
{
    vec4 _color = ubo.color * texture( tex_sampler, i_uv );
	if(_color.a == 0)
	{
		discard;
	}
	o_color = _color;
}
