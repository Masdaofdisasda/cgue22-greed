#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 lavaLevel;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
};

layout(std430, binding = 4) restrict readonly buffer Matrices
{
	mat4 modelMatrix[];
};

out vec3 fNormal;
out vec3 fPosition;
out vec2 fUV;
out flat uint matIdx;

void main()
{
	mat4 model = modelMatrix[gl_BaseInstance];
	gl_Position = ViewProj * model * vec4(vPosition, 1.0);
	fUV = vUV;
	fPosition = vec3(model * vec4(vPosition, 1.0));
	matIdx = gl_BaseInstance & 0xffff;
	fNormal = mat3(transpose(inverse(model))) * vNormal;
}