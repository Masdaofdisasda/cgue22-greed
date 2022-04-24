#version 460 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

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

out vec3 fNormal;
out vec3 fPosition;
out vec2 fUV;
out vec4 fShadow;

const mat4 scaleBias = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0);

uniform float fx = 0.7;
uniform float velx = 2.0;
uniform float ax = 0.5;
uniform float fz = 0.8;
uniform float velz = 2.1;
uniform float az = 0.4;

void main()
{
	vec4 position =  vec4(vPosition, 1.0);
	float u = fx * position.x -velx * deltaTime.y;
	position.y = ax * sin(u);
	float v = fz * position.z - velz * deltaTime.y;
	position.y += az * sin(v);
	gl_Position = ViewProj * lavaLevel * position;

	vec3 n = vec3(0.0);
	n.xy = normalize(vec2(cos(u), 1.0));
	//n.xz = normalize(vec2(cos(v), 1.0));
	fNormal = mat3(transpose(inverse(lavaLevel))) * n;
	
	fUV = vUV;

	fShadow = scaleBias * lightViewProj * lavaLevel * vec4(vPosition, 1.0);
}