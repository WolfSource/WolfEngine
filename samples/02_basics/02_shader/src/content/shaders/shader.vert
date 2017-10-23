#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

out gl_PerVertex
{
    vec4 gl_Position;
};

vec2 vertices[3] = vec2[](
                          vec2(  0.0 , -0.9),
                          vec2( -0.9 ,  0.0),
                          vec2(  0.9 ,  0.0)
                       );

void main()
{
    gl_Position = vec4( vertices[gl_VertexIndex], 0.0, 1.0 );
}
