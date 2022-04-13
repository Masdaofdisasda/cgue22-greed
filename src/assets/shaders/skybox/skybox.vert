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

out vec3 fPosition;

const vec3 pos[8] = vec3[8](
	vec3(-1.0,-1.0, 1.0),
	vec3( 1.0,-1.0, 1.0),
	vec3( 1.0, 1.0, 1.0),
	vec3(-1.0, 1.0, 1.0),

	vec3(-1.0,-1.0,-1.0),
	vec3( 1.0,-1.0,-1.0),
	vec3( 1.0, 1.0,-1.0),
	vec3(-1.0, 1.0,-1.0)
);

const int indices[36] = int[36](
	0,2,1,2,0,3,
	1,6,5,6,1,2,
	7,5,6,5,7,4,
	4,3,0,3,4,7,
	4,1,5,1,4,0,
	3,6,2,6,3,7
);

void main()
{
	int idx = indices[gl_VertexID];

	gl_Position = ViewProj * vec4(500.0*pos[idx], 1.0);
	
	fPosition = pos[idx].xyz;
}