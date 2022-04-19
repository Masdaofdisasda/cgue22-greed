#version 460 core

layout (location = 0) out vec2 uv;

const float levelBounds = 500.0;
const float sqrt3div3 = 0.57735;

const vec3 pos[3] = vec3[3](
	vec3(-levelBounds, 0.0,  -levelBounds*sqrt3div3),
	vec3(0.0, 0.0, levelBounds * sqrt3div3),
	vec3(levelBounds, 0.0, -levelBounds*sqrt3div3)
);

const vec2 uvs[3] = vec2[3](
	vec2(0.5, 1.0),
	vec2(0.0, 0.0),
	vec2(1.0, 0.0)
);

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

	gl_Position = ViewProj * lavaLevel * vec4(pos[gl_VertexID], 1.0);
	
	uv =  uvs[gl_VertexID];
}