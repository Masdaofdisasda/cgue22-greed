#version 460

// VAO Buffer data
layout (location = 10) in vec3 vPosition; // vertex position

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
	gl_Position = ViewProj * vec4(vPosition, 1.0);
}