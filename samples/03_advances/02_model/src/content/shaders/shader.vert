#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 i_position;

layout(set = 0, binding = 0) uniform U0
{
	mat4	wvp;
} u0;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec2 o_uv;

void main() 
{
    gl_Position = u0.wvp * vec4(i_position, 1.0);
}
