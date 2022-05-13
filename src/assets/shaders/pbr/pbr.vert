#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

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

layout(std430, binding = 4) restrict readonly buffer Matrices
{
	mat4 modelMatrix[];
};

out vec3 fNormal;
out vec3 fPosition;
out vec2 fUV;
out vec4 fShadow;
out flat uint mat_id;

const mat4 scaleBias = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0);

// vertex wave animation
float x_freq = 0.1;
float x_velo = 2.0;
float x_amp = 0.05;
float z_freq = 0.2;
float z_velo = 2.1;
float z_amp = 0.07;

void main()
{
	mat4 model = modelMatrix[gl_BaseInstance >> 16];
	mat_id = gl_BaseInstance & 0xffff;
	
	vec3 position =  vPosition;
	vec3 normal = vNormal;
	if(mat_id == normalMap.y)
	{
		float u = x_freq * position.x -x_velo * deltaTime.y;
		float v = z_freq * position.z - z_velo * deltaTime.y;
		position.y = x_amp * sin(u);
		position.y += z_amp * sin(v);
		
		vec3 normal =  vec3(0.0);
		normal.xy = normalize(vec2(cos(u), 1.0));
		//n.xz = normalize(vec2(cos(v), 1.0));
	}

	gl_Position = ViewProj * model * vec4(position, 1.0);
	//gl_Position = lightViewProj * model * vec4(vPosition, 1.0); // view from light
	fUV = vUV;
	fPosition = vec3(model * vec4(position, 1.0));
	fNormal = mat3(transpose(inverse(model))) * normal;
	
	fShadow = scaleBias * lightViewProj * model * vec4(position, 1.0);
}