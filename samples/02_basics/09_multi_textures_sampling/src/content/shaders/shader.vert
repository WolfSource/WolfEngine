#version 450

layout(set=0, binding=1) uniform U0
{
    float time;
} u0;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec2 o_uv;
layout(location = 1) out float o_time;

void main() 
{
    gl_Position = vec4(i_position, 1.0);
    o_uv = i_uv;
	o_time = u0.time;
}
