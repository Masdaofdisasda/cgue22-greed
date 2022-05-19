#version 460 core

layout (location = 0) out vec4 FragColor;

layout(location = 0) in vec2 uv;

layout (binding = 12) uniform sampler2D depthTex;
layout (binding = 16) uniform sampler2D sceneTex;
layout (binding = 20) uniform sampler2D bluNoise;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 lightView;
	mat4 lightViewProj;
	mat4 viewInv;
	mat4 projInv;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
    vec4 ssao1;
    vec4 ssao2;
};

uniform mat4 lightProj;

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
int numberOfRaySteps = 32;

float tau = 0.0015; // probability of collision
float phi = normalMap.z*length(dLights[0].intensity.rgb); // power of light source
float PI_RCP = 0.31830988618379067153776752674503;
const float c_goldenRatioConjugate = 0.61803398875f; // also just fract(goldenRatio)
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


float tri( float x ){ 
  return abs( fract(x) - .5 );
}

vec3 tri3( vec3 p ){
 
  return vec3( 
      tri( p.z + tri( p.y * 1. ) ), 
      tri( p.z + tri( p.x * 1. ) ), 
      tri( p.y + tri( p.x * 1. ) )
  );

}
// Taken from https://www.shadertoy.com/view/4ts3z2
// By NIMITZ  (twitter: @stormoid)
float triNoise3d(in vec3 p, in float spd, in float time)
{
    float z=1.4;
	float rz = 0.;
    vec3 bp = p;
	for (float i=0.; i<=3.; i++ )
	{
        vec3 dg = tri3(bp*2.);
        p += (dg+time*spd);

        bp *= 1.8;
		z *= 1.5;
		p *= 1.2;
        //p.xz*= m2;
        
        rz+= (tri(p.z+tri(p.x+tri(p.y))))/z;
        bp += 0.14;
	}
	return rz;
}

float sample_fog(vec3 pos) {
	return triNoise3d(pos * 2.2 / 8, 0.2, deltaTime.x)*0.75;
}

void main() {
 
	float depth = texture(sceneTex, uv).r;
	
	vec3 frag_pos = CalculateWorldPosition(uv, depth);

	//Calculate the ray properties.
	vec4 end_pos_worldspace  = viewPos;
	vec4 start_pos_worldspace = vec4(frag_pos, 1.0);
	vec4 delta_worldspace = normalize(end_pos_worldspace - start_pos_worldspace);
	
	vec4 end_pos_lightview =  lightView * end_pos_worldspace;
	vec4 start_pos_lightview =  lightView * start_pos_worldspace;
	vec4 delta_lightview = normalize(end_pos_lightview - start_pos_lightview);
	
	float raymarch_distance_lightview = length(end_pos_lightview - start_pos_lightview);
	float step_size_lightview = raymarch_distance_lightview / numberOfRaySteps;
	
	float raymarch_distance_worldspace = length(end_pos_worldspace - start_pos_worldspace);
	float step_size_worldspace = raymarch_distance_worldspace / numberOfRaySteps;
	
        // blue noise
    float dither_value = texture(bluNoise, uv).r;
    dither_value = fract(dither_value + deltaTime.x * c_goldenRatioConjugate);
	vec4 ray_position_lightview = start_pos_lightview + dither_value *step_size_lightview * delta_lightview;
	vec4 ray_position_worldspace = start_pos_worldspace + dither_value * step_size_worldspace * delta_worldspace;

	//Perform the ray.
	float light_contribution = 0.0;
	for (float l = raymarch_distance_worldspace; l > step_size_worldspace; l -= (step_size_worldspace)) {
		vec4 ray_position_lightspace = lightProj * vec4(ray_position_lightview.xyz, 1);
		// perform perspective divide            
		vec3 proj_coords = ray_position_lightspace.xyz / ray_position_lightspace.w;
		
		// transform to [0,1] range
		proj_coords = proj_coords * 0.5 + 0.5;		
		
		// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
		vec4 closest_depth;

		closest_depth = texture(depthTex, proj_coords.xy);
		
		float shadow_term = 1.0;
		
		if (proj_coords.z > closest_depth.r) {
			shadow_term = 0.0;
		}

		vec3 lightPos = vec3(-7,60,-14); // directional light has no position
		float d = length(ray_position_worldspace.xyz - lightPos);
		float d_rcp = 1.0/d;
		
		float fog = sample_fog(ray_position_worldspace.xyz);
		
		light_contribution += fog * tau * (shadow_term * (phi * 0.25 * PI_RCP) * d_rcp * d_rcp ) *exp(-d*tau) * exp(-l*tau) * step_size_worldspace;
	
		ray_position_lightview += step_size_lightview * delta_lightview;
		ray_position_worldspace += step_size_worldspace * delta_worldspace;
	}

	vec3 volumetricLight = light_contribution.rrr;

	FragColor = vec4(volumetricLight, 1.0f);
}