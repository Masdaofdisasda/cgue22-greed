#version 460 core

layout (location = 0) out vec4 FragColor;

in vec3 fPosition;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 lavaLevel;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
};

layout (binding = 8) uniform samplerCube environmentTex;

void main()
{
    FragColor = texture(environmentTex, fPosition, 0.0);
};