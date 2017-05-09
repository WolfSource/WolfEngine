//https://math.stackexchange.com/questions/13272/how-would-i-create-a-rotation-matrix-that-rotates-x-by-a-y-by-b-and-z-by-c
#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set=0, binding=1) uniform u_buffer
{
    mat4 wvp;
} U0;

// vertex input attributes
layout(location = 0) in vec4 i_position;
layout(location = 1) in vec2 i_uv;

//vertex instance attributes
layout (location = 2) in vec3 i_instance_pos;
layout (location = 3) in vec3 i_instance_rot;
layout (location = 4) in float i_instance_scale;
layout (location = 5) in int i_instance_uv_index;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

void main() 
{
	mat3 mx, my, mz;
	
	float _s = sin(instanceRot.x + ubo.locSpeed);
	float _c = cos(instanceRot.x + ubo.locSpeed);

	mx[0] = vec3(1.0, 0.0, 0.0);
	mx[1] = vec3(0.0, c, 0.0);
	mx[2] = vec3(0.0, 0.0, 1.0);

    gl_Position = U0.wvp * i_position;
    v_Texcoord = i_uv;
}
