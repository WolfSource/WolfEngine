#version 450

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

layout(push_constant) uniform PushConstants 
{
	vec4 diffuse_color;
} p0;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec2 o_uv;
layout(location = 1) out vec4 o_diffuse_color;

void main() 
{
    gl_Position = vec4(i_position, 1.0);
    o_uv = i_uv;
	o_diffuse_color = p0.diffuse_color;
}
