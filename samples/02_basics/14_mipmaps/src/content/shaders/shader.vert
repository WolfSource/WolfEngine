#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

layout(set = 0, binding = 0) uniform U0
{
	float	texture_lod;
} u0;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec2 o_uv;
layout(location = 1) out float o_texture_lod;
layout(location = 2) flat out int o_sampler_index;//flat for int

void main() 
{
    gl_Position = vec4(i_position, 1.0);
    o_uv = i_uv;
	o_texture_lod = u0.texture_lod;
}
