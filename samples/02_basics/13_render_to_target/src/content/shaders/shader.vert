#version 450

layout(set=0, binding=1) uniform u0
{
    mat4 wvp;
};

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 o_uv;

void main() 
{
    gl_Position = wvp * vec4(i_position, 1.0);
    o_uv = i_uv;
}
