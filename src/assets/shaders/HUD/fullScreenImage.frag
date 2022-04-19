#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 9) uniform sampler2D texScene;

void main()
{
	outColor = texture(texScene, uv);
}