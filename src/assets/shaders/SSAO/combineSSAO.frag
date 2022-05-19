#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 14) uniform sampler2D texlight;
layout(binding = 16) uniform sampler2D texScene;
layout(binding = 17) uniform sampler2D texSSAO;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 lavaLevel;
	mat4 lightViewProj;
	mat4 viewInv;
	mat4 projInv;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
    vec4 ssao1;
    vec4 ssao2;
};

void main()
{
	float scale = ssao1.x;
	float bias = ssao1.y;
	
	vec4 color = texture(texScene, uv);
	vec4 light = texture(texlight, uv);
	float ssao = clamp( texture(texSSAO,  uv).r + bias, 0.0, 1.0 );
	
	outColor = vec4(
		mix(color+light, color * ssao, scale).rgb,
		1.0
	);
}