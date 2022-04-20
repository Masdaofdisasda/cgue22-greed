#version 460

layout (location = 0) in vec4 VertexPosition;

out vec3 Position;

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
    Position = (ViewProj * VertexPosition).xyz;
    gl_Position = ViewProj * VertexPosition;
}