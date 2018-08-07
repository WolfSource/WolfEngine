#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform UBO 
{
	mat4 wvp;
} u0; 

layout (triangles) in;

layout (location = 0) in vec2 i_uv[];

layout (location = 0) out vec2 o_uv;

void main(void)
{
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                  (gl_TessCoord.y * gl_in[1].gl_Position) +
                  (gl_TessCoord.z * gl_in[2].gl_Position);
	gl_Position = u0.wvp * gl_Position;
	o_uv = gl_TessCoord.x * i_uv[0] + gl_TessCoord.y * i_uv[1] + gl_TessCoord.z * i_uv[2];
}