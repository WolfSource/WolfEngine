#version 450

layout(set=0, binding=0) uniform sampler2D t_sampler;

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main() 
{
  o_color = texture( t_sampler, i_uv );
}
