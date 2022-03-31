#version 460

// VAO Buffer data
layout (location = 10) in vec3 vPosition; // vertex position

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 ViewProjSkybox;
};

void main()
{
	gl_Position = ViewProj * vec4(vPosition, 1.0);
}