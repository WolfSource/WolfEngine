#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (triangles, fractional_odd_spacing, ccw) in;

layout (binding = 1) uniform UBO
{
    mat4 projection_view;
	mat4 model;
} ubo;

layout(location = 0) in vec2 i_uv[];
layout(location = 0) out vec2 o_uv;

void main()
{
    vec4 _position = 
			(gl_TessCoord.x * gl_in[0].gl_Position) +
            (gl_TessCoord.y * gl_in[1].gl_Position) +
            (gl_TessCoord.z * gl_in[2].gl_Position);

	gl_Position = ubo.projection_view * ubo.model * _position;
	o_uv  = gl_TessCoord[0] * i_uv[0] + gl_TessCoord[1] * i_uv[1] + gl_TessCoord[2] * i_uv[2];
}

