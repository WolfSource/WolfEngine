#version 450

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec4 o_color;

void main()
{
    gl_Position = vec4(i_position, 1.0);
    o_color = i_color;
}
