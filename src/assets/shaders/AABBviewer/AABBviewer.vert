#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 lavaLevel;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
    vec4 ssao1;
    vec4 ssao2;
};

uniform vec3 min;
uniform vec3 max;

const int indices[36] = int[36](
	0, 1, 2, 2, 3, 0,
	1, 5, 6, 6, 2, 1,
	7, 6, 5, 5, 4, 7,
	4, 0, 3, 3, 7, 4,
	4, 5, 1, 1, 0, 4,
	3, 2, 6, 6, 7, 3
);

void main()
{

	const vec3 pos[8] = vec3[8](
	vec3(min.x, min.y, max.z),
	vec3(max.x, min.y, max.z),
	vec3(max.x, max.y, max.z),
	vec3(min.x, max.y, max.z),

	vec3(min.x, min.y, min.z),
	vec3(max.x, min.y, min.z),
	vec3(max.x, max.y, min.z),
	vec3(min.x, max.y, min.z)
	);

	int idx = indices[gl_VertexID];

	gl_Position = ViewProj * vec4(pos[idx], 1.0);
}