#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform sampler2D tex_sampler;

layout (binding = 2) uniform UBO
{
	vec4 color;
} i_ubo;

layout (location = 0) in vec3 i_norm;
layout (location = 1) in vec2 i_uv;

layout (location = 0) out vec4 o_color;

vec3 calculate_ambient(vec3 pNormal, vec3 pColor)
{
	// Convert from [-1, 1] to [0, 1]
	float up = pNormal.y * 0.5 + 0.5;

	const vec3 _ambient_lower_color = vec3(0.5, 0.5, 0.5);
	const vec3 _ambient_upper_color = vec3(2.0, 2.0, 2.0);

	// Calculate the ambient value
	vec3 _ambient = _ambient_lower_color + up *_ambient_upper_color;

	// Apply the ambient value to the color
	return _ambient * pColor;
}

void main()
{
	vec4 _diffuse_color =  i_ubo.color * texture( tex_sampler, i_uv );
	if (_diffuse_color.a == 0)
	{
		discard;
	}

	_diffuse_color *= _diffuse_color;
	vec3 _ambient_color = calculate_ambient(i_norm, _diffuse_color.rgb);
	o_color = vec4(_ambient_color, _diffuse_color.a);
}
