#version 460

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec4 ParticlePosition;

out vec3 Position;
//out vec2 UV;

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

const mat4 model = mat4(
1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
ParticlePosition.x, ParticlePosition.y, ParticlePosition.z, 1.0);


void main()
{
	Position = (ViewProj * model * vec4(0,0,0,1)).xyz;
	gl_Position = ViewProj * model * vec4(0,0,0,1);
	//Position = vec3(ViewProj * model * vec4(vPosition,0.0 ,1.0));
    //gl_Position = vec4(Position,1.0);

	//UV = vec2(0);//vPosition.xy;
}