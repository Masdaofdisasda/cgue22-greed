#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 16) uniform sampler2D texScene;

void main()
{
	float gamma = 2.2;
	vec4 color = texture(texScene, uv);
	outColor = vec4(pow( color.xyz, vec3(1.0/gamma) ), color.w) ;
}