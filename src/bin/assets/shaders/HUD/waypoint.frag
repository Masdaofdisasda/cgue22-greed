#version 460 core

layout(location = 1) in vec3 pos;
layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 16) uniform sampler2D waypoint;

void main()
{
	vec4 color = texture(waypoint, uv);
	color.rgb = color.rgb * vec3(.95f, .86f, .6f);
	outColor = color ;
}