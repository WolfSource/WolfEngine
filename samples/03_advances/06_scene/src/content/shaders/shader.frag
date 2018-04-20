#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform sampler2D tex_sampler;
layout (binding = 2) uniform U1
{
	float cmds;
	float padding_0;
	float padding_1;
	float padding_2;
} u1;

layout (location = 0) in vec3 i_norm;
layout (location = 1) in vec2 i_uv;
layout(location = 2) in vec3 i_color;

layout(location = 0) out vec4 o_color;

vec3 calculate_ambient(vec3 pNormal, vec3 pColor)
{
	// Convert from [-1, 1] to [0, 1]
	float up = pNormal.y * 0.5 + 0.5;

	const vec3 _ambient_lower_color = vec3(0.5, 0.5, 0.5);
	const vec3 _ambient_upper_color = vec3(1.0, 1.0, 1.0);

	// Calculate the ambient value
	vec3 _ambient = _ambient_lower_color + up *_ambient_upper_color;

	// Apply the ambient value to the color
	return _ambient * pColor;
}

void main() 
{
	vec3 _diffuse_color;
	if (u1.cmds == 1)
	{
		_diffuse_color = i_color;
	}
	else
	{
		_diffuse_color = vec3(1, 1, 1);
	}
	vec4 _texture_color =  texture( tex_sampler, i_uv );
	if (_texture_color.a > 0)
	{
		_diffuse_color *= _texture_color.rgb;
	}

	_diffuse_color *=  2 * _diffuse_color;

	vec3 _ambient_color = calculate_ambient(i_norm, _diffuse_color);
	o_color = vec4(_ambient_color, 1.0);
}
