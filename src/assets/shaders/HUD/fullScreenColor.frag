#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

uniform vec4 color;

void main()
{
	outColor = color;
}