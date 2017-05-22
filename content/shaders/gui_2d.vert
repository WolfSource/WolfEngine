#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec4 i_color;
layout(location = 2) in vec2 i_uv;

//vertex instance attributes
layout (location = 3) in vec2 i_instance_pos;
layout (location = 4) in vec4 i_instance_color;
layout (location = 5) in vec2 i_instance_uv;
//i_instance_index = 0 means this is ref model and other instances has a value greater than 1
layout (location = 6) in int  i_instance_index;

layout(location = 0) out vec4 o_color;
layout(location = 1) out vec2 o_uv;

void main() 
{
	if (i_instance_index == 0)
	{
		//ref mesh
		gl_Position = vec4(vec3(i_position, 0.5f), 1);
		o_color = i_color;
		o_uv = i_uv;
	}
	else
	{
		//instance mesh
		gl_Position = vec4(vec3(i_instance_pos, 0.5f), 1);
		o_color = i_instance_color;
		o_uv = i_instance_uv;
	}
}
