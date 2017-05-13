#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(binding = 0) uniform UBO
{
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

layout(location = 0) out vec3 outColor;


out gl_PerVertex
{
    vec4 gl_Position;
};


void main()
{
    gl_Position = vec4(inPosition, 1.0);
    outColor = inColor;
}