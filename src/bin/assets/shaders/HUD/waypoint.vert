#version 460 core

layout (location=1) out vec3 pos;
layout (location=0) out vec2 uv;

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

uniform vec3 position = vec3(0.0, 7.0, 0.0);
float scale = .7;

mat4 model = mat4(
scale, 0.0, 0.0, 0.0,
0.0, scale, 0.0, 0.0,
0.0, 0.0, scale, 0.0,
position.x, position.y, position.z, 1.0);

void main()
{
	float u = float( ((uint(gl_VertexID) + 2u) / 3u) % 2u );
	float v = float( ((uint(gl_VertexID) + 1u) / 3u) % 2u );

	vec4 CameraRight_worldspace = viewInv[0];
	vec4 CameraUp_worldspace = viewInv[1];

	vec4 vertex = vec4(-1.0+u*2.0, -1.0+v*2.0, 0.0, 1.0);

	vec3 vertex_worldspace =
    vec3(0) //center
    + CameraRight_worldspace.xyz * vertex.x
    + CameraUp_worldspace.xyz * vertex.y;
	
	pos = (ViewProj * model * vec4(vertex_worldspace,1)).xyz;
	gl_Position = ViewProj * model * vec4(vertex_worldspace,1);
	uv = vec2(u, v);
}
