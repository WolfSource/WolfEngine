#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set=0, binding=0) uniform sampler2D u_samplers;

layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;

void main()
{
    o_color = texture( u_samplers, i_uv );
}
