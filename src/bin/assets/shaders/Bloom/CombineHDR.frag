/**/
#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 16) uniform sampler2D texScene;
layout(binding = 17) uniform sampler2D texLuminance;
layout(binding = 18) uniform sampler2D texBloom;
layout(binding = 13) uniform sampler3D Lut3D;

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

	float maxWhite = bloom.y;

// Extended Reinhard tone mapping operator
vec3 Reinhard2(vec3 x)
{
	return (x * (1.0 + x / (maxWhite * maxWhite))) / (1.0 + x);
}

vec3 lookUp(vec3 raw)
{
	float n = 32;
	vec3 lutSize = vec3(n,n,n); //lut resolution

	vec3 scale = (lutSize -1.0) / lutSize;
	vec3 offset = 1.0/(2.0*lutSize);
	
	return max(texture(Lut3D, scale*raw + offset).rgb, raw);
}

void main()
{
	float exposure = bloom.x;
	float bloomStrength = bloom.z;
	
	vec3 color = texture(texScene, uv).rgb;
	vec3 bloom = texture(texBloom, uv).rgb;
	float avgLuminance = texture(texLuminance, vec2(0.5, 0.5)).x;

	float midGray = 0.5;

	color *= exposure * midGray / avgLuminance; //use smallest float instead of 0
	color = Reinhard2(color);
	color = lookUp(color);
	color = color + bloomStrength * bloom;

	outColor = vec4(color,1.0f);
}