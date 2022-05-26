#version 460

layout (location = 0) in vec2 vPosition;

out vec3 Position;
out vec2 UV;

layout(std430, binding=0) buffer Pos {
  vec4 pos[];
};

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

vec4 particlePos = pos[gl_InstanceID];
float scale = 0.4;
vec4 CameraRight_worldspace = viewInv[0];
vec4 CameraUp_worldspace = viewInv[1];


const mat4 model = mat4(
scale, 0.0, 0.0, 0.0,
0.0, scale, 0.0, 0.0,
0.0, 0.0, scale, 0.0,
particlePos.x, particlePos.y, particlePos.z, 1.0);


void main()
{
	vec3 vertexPosition_worldspace =
    vec3(0) //center
    + CameraRight_worldspace.xyz * vPosition.x
    + CameraUp_worldspace.xyz * vPosition.y;

	Position = (ViewProj * model * vec4(vertexPosition_worldspace,1)).xyz;
	gl_Position = ViewProj * model * vec4(vertexPosition_worldspace,1);

	UV = (vPosition.xy +1)*0.5;
}