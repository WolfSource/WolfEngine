#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 1) uniform sampler2D t_sampler;
//layout (set = 0, binding = 1) uniform texture2D t_color;
//layout (set = 0, binding = 2) uniform sampler t_sampler;

layout(location = 0) in vec2 i_uv;
layout(location = 1) in float i_texture_lod;
layout(location = 2) flat in int i_sampler_index;

layout(location = 0) out vec4 o_color;

void main() 
{
	o_color = texture(t_sampler, i_uv, i_texture_lod);
}
