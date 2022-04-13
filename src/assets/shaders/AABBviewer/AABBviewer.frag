#version 460 core

layout (location = 0) out vec4 FragColor;

uniform vec4 lineColor;

void main()
{
    FragColor = lineColor;
};