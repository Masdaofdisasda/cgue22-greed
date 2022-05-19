#version 460 core

layout (location = 0) out vec4 FragColor;

layout(location = 0) in vec2 uv;

layout (binding = 12) uniform sampler2D depthTex;
layout (binding = 16) uniform sampler2D sceneTex;

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

// light sources
struct DirectionalLight
{
	vec4 direction;

	vec4 intensity;
};

layout (std140, binding = 1) uniform dLightUBlock {
 DirectionalLight dLights [ dMAXLIGHTS ]; // xMAXLIGHTS gets replaced at runtime
};

// not used but is needed for shader compilatioin
struct PositionalLight
{
	vec4 position;
    vec4 intensity;
};
uniform int numPos;

layout (std140, binding = 2) uniform pLightUBlock {
 PositionalLight pLights [ pMAXLIGHTS ];
};

// code from: https://github.com/metzzo/ezg17-transition
// https://satellitnorden.wordpress.com/2018/09/09/vulkan-adventures-part-5-rayloaded-simple-volumetric-fog/
// https://www.shadertoy.com/view/WsfBDf
float zFar = ssao1.w;
float density = 0.002f;
float rayDistanceSquared = zFar*zFar;
int numberOfRaySteps = 16;
const mat4 scaleBias = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0);

/*
*   Calculates the world position given a texture coordinate and depth.
*/
vec3 CalculateWorldPosition(vec2 textureCoordinate, float depth)
{
	float z = depth* 2.0 - 1.0;
    vec4 fragmentScreenSpacePosition = vec4(textureCoordinate * 2.0f - 1.0f, z, 1.0);
    vec4 viewSpacePosition = projInv * fragmentScreenSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = viewInv * viewSpacePosition;

    return worldSpacePosition.xyz;
}
 
/*
*   Returns the length of a vector squared.
*/
float LengthSquared(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

void main() {
 
	float depth = texture(sceneTex, uv).r;
	
	vec3 frag_pos = CalculateWorldPosition(uv, depth);

	//Calculate the ray properties.
	vec3 rayDirection = normalize(frag_pos - viewPos.xyz);
	float distanceToScenePositionSquared = LengthSquared(frag_pos - viewPos.xyz);
	float rayDistance = min(rayDistanceSquared, distanceToScenePositionSquared);
	vec3 rayStep = rayDirection * (sqrt(rayDistance) / numberOfRaySteps);
	float densityPerStep = density / numberOfRaySteps;
	vec3 currentPosition = frag_pos;
	float startRayOffset = 0; //TODO

	//Perform the ray.
	float fogLitPercent = 0.0f;
    for (int i = 0; i < numberOfRaySteps; ++i)
    {
        vec3 testPos = currentPosition + rayStep * float(i);
		
		vec4 lightSpacePos = scaleBias * lightViewProj * vec4(testPos, 1.0);
		//vec4 lightSpacePos = lightViewProj * vec4(testPos, 1.0);
		vec3 shadowCoord = lightSpacePos.xyz/lightSpacePos.w;

        float test = texture(depthTex, shadowCoord.xy).r;
		test = test < shadowCoord.z ? 0 : 1.0;
        fogLitPercent += test;
    }
	fogLitPercent/=numberOfRaySteps;

	vec3 volumetricLight = fogLitPercent * dLights[0].intensity.rgb * 0.02;

	FragColor = vec4(volumetricLight, 1.0f);
}
/*
void main() {
 
	float depth = texture(sceneTex, uv).r;
	
	vec3 frag_pos = CalculateWorldPosition(uv, depth);

	//Calculate the ray properties.
	vec3 rayDirection = normalize(frag_pos - viewPos.xyz);
	float distanceToScenePositionSquared = LengthSquared(frag_pos - viewPos.xyz);
	float rayDistance = min(rayDistanceSquared, distanceToScenePositionSquared);
	vec3 rayStep = rayDirection * (sqrt(rayDistance) / numberOfRaySteps);
	float densityPerStep = density / numberOfRaySteps;
	vec3 currentPosition = frag_pos;
	float startRayOffset = 0; //TODO

	//Perform the ray.
	float fogLitPercent = 0.0f;
    for (int i = 0; i < numberOfRaySteps; ++i)
    {
        vec3 testPos = currentPosition + rayStep * float(i);
		
		//vec4 lightSpacePos = scaleBias * lightViewProj * vec4(testPos, 1.0);
		vec4 lightSpacePos = lightViewProj * vec4(testPos, 1.0);
		vec3 shadowCoord = lightSpacePos.xyz/lightSpacePos.w;

        float test = texture(depthTex, shadowCoord.xy).r;
		test = test < shadowCoord.z ? 0 : 1.0;
        fogLitPercent += test;
    }

	FragColor = vec4(fogLitPercent.rrr, 1.0f);
}
*/