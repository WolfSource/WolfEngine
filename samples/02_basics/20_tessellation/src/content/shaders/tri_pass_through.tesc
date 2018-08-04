#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (vertices = 3) out;

layout (location = 0) in vec2 i_uv[];

layout (location = 0) out vec2 o_uv[3];

void main()
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = 10.0; 
		gl_TessLevelOuter[0] = 1.0; gl_TessLevelOuter[1] = 10.0; gl_TessLevelOuter[2] = 1.0;
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	o_uv[gl_InvocationID] = i_uv[gl_InvocationID];	
}