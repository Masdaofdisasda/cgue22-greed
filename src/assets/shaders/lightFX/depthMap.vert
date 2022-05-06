#version 460 core

layout (location = 0) in vec3 vPosition;
//layout (location = 1) in vec3 vNormal;
//layout (location = 2) in vec2 vUV;

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

layout(std430, binding = 4) restrict readonly buffer Matrices
{
	mat4 modelMatrix[];
};

void main()
{
	mat4 model = modelMatrix[gl_BaseInstance >> 16];
	gl_Position = lightViewProj * model * vec4(vPosition, 1.0);
}