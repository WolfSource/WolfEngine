#version 450

layout(set=0, binding=0) uniform sampler2DArray t_sampler_array;
layout(set=0, binding=1) uniform u0
{
    int index;
};

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main() 
{
	o_color = texture( t_sampler_array, vec3(i_uv, index) );
}
