#version 460 core

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

uniform vec3 corner0;
uniform vec3 corner1;
uniform vec3 corner2;
uniform vec3 corner3;
uniform vec3 corner4;
uniform vec3 corner5;
uniform vec3 corner6;
uniform vec3 corner7;

const int indices[36] = int[36](
	0, 2, 1, 2, 0, 3,
	1, 6,5, 6, 1,2,
	7, 5,6, 5, 7,4,
	4, 3,0, 3, 4,7,
	4, 1,5 , 1, 4, 0,
	3, 6, 2, 6, 3, 7
);

void main()
{

	const vec3 pos[8] = vec3[8](
	corner0,
	corner1,
	corner2,
	corner3,
	corner4,
	corner5,
	corner6,
	corner7
	);

	int idx = indices[gl_VertexID];

	gl_Position = ViewProj * vec4(pos[idx], 1.0);
}