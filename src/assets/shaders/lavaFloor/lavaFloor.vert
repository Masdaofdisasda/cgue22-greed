#version 460 core

layout (location = 0) out vec2 uv;

const vec3 pos[3] = vec3[3](
	vec3(0.0, 0.0, 100.0),
	vec3(100.0, 0.0, -100.0),
	vec3(-100.0, 0.0, -100.0)
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
	mat4 ViewProjSkybox;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
};

void main()
{

	gl_Position = ViewProj *  vec4(pos[gl_VertexID], 1.0);
	
	uv =  uvs[gl_VertexID];
}