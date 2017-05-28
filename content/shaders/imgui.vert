#version 450

layout (location = 0) in vec2 i_position;
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec4 i_color;

layout (push_constant) uniform PushConstants 
{
	vec2 scale;
	vec2 translate;
	int  sampler_index;
} push_0;

layout (location = 0) out vec3 o_uv_image_index;
layout (location = 2) out vec4 o_color;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
	o_uv_image_index.xy = i_uv;
	o_uv_image_index.z = push_0.sampler_index;
	o_color = i_color;
	gl_Position = vec4(i_position * push_0.scale + push_0.translate, 0.0, 1.0);
}