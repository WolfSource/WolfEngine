#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform sampler2D tex_sampler;

layout (location = 0) in vec2 i_uv;

layout (location = 0) out vec4 o_color;

void main()
{
    vec4 _color = texture( tex_sampler, i_uv );
	if(_color.a == 0)
	{
		discard;
	}
	o_color = _color;
}
