#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define ID gl_InvocationID

layout(push_constant) uniform PushConstants 
{
	float inner_value;
} p0;

layout (vertices = 3) out;

layout (location = 0) in vec2 i_uv[];

layout (location = 0) out vec2 o_uv[3];

void main()
{
    if (ID == 0)
    {
        gl_TessLevelInner[0] = p0.inner_value;
		gl_TessLevelOuter[0] = p0.inner_value; 
		gl_TessLevelOuter[1] = p0.inner_value; 
		gl_TessLevelOuter[2] = p0.inner_value; 
    }

    gl_out[ID].gl_Position = gl_in[ID].gl_Position;
	o_uv[ID] = i_uv[ID];	
}