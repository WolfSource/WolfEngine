#version 450

layout(set=0, binding=0) uniform sampler2DArray t_sampler_array;

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main() 
{
	//get the color of first texture
	vec4 _c1 =  texture( t_sampler_array, vec3(i_uv, 0) );

	//get the color of second texture
    vec4 _c2 =  texture( t_sampler_array, vec3(i_uv, 1) );
    
	//blend the two colors together and apply gamma value
	const float _gamma = 2.0;
    vec4 _blend_color = _c1 * _c2 * _gamma;
    
    //saturate the final color
	//o_color = saturate(_blend_color);//for HLSL
    o_color = clamp(_blend_color, 0.0, 1.0);//for GLSL
}
