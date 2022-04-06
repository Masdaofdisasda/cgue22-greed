#version 460 core

layout (location = 0) out vec4 FragColor;

in vec3 fPosition;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 ViewProjSkybox;
	vec4 bloom;
    vec4 normalMap;
};

uniform samplerCube environment;

void main()
{
	vec3 color = textureLod(environment, vec3(fPosition), 0.0).xyz;
    
    FragColor = vec4(color, 1.0);
};