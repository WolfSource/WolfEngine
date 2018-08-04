#version 450

layout(binding=0) uniform sampler2D t_sampler;

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main() 
{
  o_color = vec4(1,0,0,1);//texture( t_sampler, i_uv );
}
