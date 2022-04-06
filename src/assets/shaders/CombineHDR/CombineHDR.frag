/**/
#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 9) uniform sampler2D texScene;
layout(binding = 10) uniform sampler2D texLuminance;
layout(binding = 11) uniform sampler2D texBloom;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 lavaLevel;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
};

// Extended Reinhard tone mapping operator
vec3 Reinhard2(vec3 x)
{
	float maxWhite = bloom.y;

	return (x * (1.0 + x / (maxWhite * maxWhite))) / (1.0 + x);
}

void main()
{
	float exposure = bloom.x;
	float bloomStrength = bloom.z;

	vec3 color = texture(texScene, uv).rgb;
	vec3 bloom = texture(texBloom, uv).rgb;
	float avgLuminance = texture(texLuminance, vec2(0.5, 0.5)).x;

	float midGray = 0.5;

	color *= exposure * midGray / (avgLuminance + 0.001);
	color = Reinhard2(color);
	outColor = vec4(color + bloomStrength * bloom, 1.0);
}