#version 450

layout(set=0, binding=0) uniform sampler2D t_sampler;

layout(location = 0) in vec2 i_uv;
layout(location = 1) in vec4 i_diffuse;

layout(location = 0) out vec4 o_color;

void main() 
{
	o_color = i_diffuse * texture( t_sampler, i_uv );
}
