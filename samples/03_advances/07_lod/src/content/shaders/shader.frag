#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 2) uniform sampler2D tex_sampler;
layout (binding = 3) uniform U2
{
	float cmds;
	float padding_0;
	float padding_1;
	float padding_2;
} u2;

layout (location = 0) in vec3 i_norm;
layout (location = 1) in vec2 i_uv;
layout (location = 2) in flat uint i_texture_mip_map_level;
layout (location = 3) in vec3 i_color;
layout (location = 4) out vec3 i_camera_pos;
layout (location = 5) out vec3 i_world_pos;

layout(location = 0) out vec4 o_color;

float saturate(float pValue)
{
	return clamp(pValue, 0.0f, 1.0f);
}

vec3 saturate(vec3 pValue)
{
	return vec3(
		clamp(pValue.x, 0.0f, 1.0f),
		clamp(pValue.y, 0.0f, 1.0f),
		clamp(pValue.z, 0.0f, 1.0f));
}

vec3 calculate_phong(vec3 pLightDir, vec3 pNormal, vec3 pColor)
{
	const vec3 spec_color = vec3(1.0f, 1.0f, 1.0f);
	const float spec_pow = 300.0f;
	
	vec3 norm = vec3(pNormal.x, pNormal.y, 1 - pNormal.z);
	vec3 N = normalize(norm);
	vec3 L = normalize(pLightDir);
	vec3 diffuse = max(dot(N, L), 0.0) * pColor;
	
	vec3 _h = normalize(normalize(i_camera_pos - i_world_pos) - pLightDir);
	float specular = pow(saturate(dot(_h, N)), spec_pow);
	return vec3(diffuse + spec_color * specular);
}

vec3 calculate_ambient(vec3 pNormal, vec3 pColor)
{
	// Convert from [-1, 1] to [0, 1]
	float up = pNormal.y * 0.5 + 0.5;

	const vec3 _ambient_lower_color = vec3(0.5, 0.5, 0.5);
	const vec3 _ambient_upper_color = vec3(0.0, 1.0, 1.0);

	// Calculate the ambient value
	vec3 _ambient = _ambient_lower_color + up *_ambient_upper_color;

	// Apply the ambient value to the color
	return _ambient * pColor;
}

void main() 
{
	vec3 _diffuse_color;
	if (u2.cmds == 1)
	{
		_diffuse_color = i_color;
	}
	else if (u2.cmds == 2)
	{
		//sky
		o_color = texture( tex_sampler, i_uv, 0);
		return;
	}
	else
	{
		_diffuse_color = vec3(1, 1, 1);
	}
	vec4 _texture_color =  texture( tex_sampler, i_uv, i_texture_mip_map_level);
	if (_texture_color.a > 0)
	{
		_diffuse_color *= _texture_color.rgb;
	}

	//_diffuse_color *=  2 * _diffuse_color;

	vec3 _ambient = _diffuse_color * 0.6f;
	vec3 _light_color_1 = calculate_phong(vec3( 1.0f, 1.0f,  1.0f), i_norm, _diffuse_color);
	vec3 _light_color_2 = calculate_phong(vec3(-1.0f, 1.0f, -1.0f), i_norm, _diffuse_color);
	o_color = vec4(_ambient + _light_color_1 + _light_color_2, 1.0);
}
