#version 450

layout(set=0, binding=0) uniform sampler2DArray t_sampler_array;

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main() 
{
	vec4 _t1 =  texture( t_sampler_array, vec3(i_uv, 0) );
    vec4 _t2 =  texture( t_sampler_array, vec3(i_uv, 1) );
    
    o_color = _t1 * _t2;
}
