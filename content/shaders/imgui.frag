#version 450

layout (binding = 0) uniform sampler2D font_sampler;
layout (binding = 1) uniform sampler2D images_sampler;

layout (location = 0) in vec3 i_uv_image_index;
layout (location = 2) in vec4 i_color;

layout (location = 0) out vec4 o_color;

void main() 
{
	if(i_uv_image_index.z == 0)
	{
		//font
		o_color = i_color * texture(font_sampler, i_uv_image_index.xy);
	}
	else if(i_uv_image_index.z == 1)
	{
		//icons
		o_color = i_color * texture(images_sampler, i_uv_image_index.xy);
	}

	if(o_color.a == 0) discard;	
}