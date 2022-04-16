#version 460 core

layout (location = 0) out vec4 FragColor;

layout(location = 0) in vec2 uv;

layout (binding = 12) uniform sampler2D depthTex;

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

struct PositionalLight
{
	vec4 position;
    vec4 intensity;
};

layout (std140, binding = 2) uniform pLightUBlock {
 PositionalLight pLights [ pMAXLIGHTS ];
};

// https://github.com/metzzo/ezg17-transition

float time = deltaTime.x;
float dither_pattern[16] = float[16] (
	0.0f, 0.5f, 0.125f, 0.625f,
	0.75f, 0.22f, 0.875f, 0.375f,
	0.1875f, 0.6875f, 0.0625f, 0.5625,
	0.9375f, 0.4375f, 0.8125f, 0.3125
);
float bias = 0.1;
float tau = 20000.0;
float phi = 0.00001;
int numSamples = 64;
#define PI (0.31830988618379067153776752674503)

vec3 world_pos_from_depth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clip_space_position = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 view_space_position = projInv * clip_space_position;

    // Perspective divide
    view_space_position /= view_space_position.w;

    vec4 world_space_position = viewInv * view_space_position;
    return world_space_position.xyz;
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
	return triNoise3d(pos * 2.2 / 8, 0.2, time)*0.75;
}

float volumetric_lighting_directional(vec3 frag_pos, DirectionalLight light) {

	float dither_value = dither_pattern[ (int(gl_FragCoord.x) % 4)* 4 + (int(gl_FragCoord.y) % 4) ];
	int num_samples = numSamples;
	
	vec4 end_pos_worldspace  = viewPos;
	vec4 start_pos_worldspace = vec4(frag_pos, 1.0);
	vec4 delta_worldspace = normalize(end_pos_worldspace - start_pos_worldspace);
	
	vec4 end_pos_lightview = lightViewProj * end_pos_worldspace;
	vec4 start_pos_lightview = lightViewProj * start_pos_worldspace;
	vec4 delta_lightview = normalize(end_pos_lightview - start_pos_lightview);
	
	float raymarch_distance_lightview = length(end_pos_lightview - start_pos_lightview);
	float step_size_lightview = raymarch_distance_lightview / num_samples;
	
	float raymarch_distance_worldspace = length(end_pos_worldspace - start_pos_worldspace);
	float step_size_worldspace = raymarch_distance_worldspace / num_samples;
	
	vec4 ray_position_lightview = start_pos_lightview + dither_value*step_size_lightview * delta_lightview;
	vec4 ray_position_worldspace = start_pos_worldspace + dither_value*step_size_worldspace * delta_worldspace;
	
	float light_contribution = 0.0;
	for (float l = raymarch_distance_worldspace; l > step_size_worldspace; l -= step_size_worldspace) {
		vec4 ray_position_lightspace = lightViewProj * vec4(ray_position_lightview.xyz, 1);
		// perform perspective divide            
		vec3 proj_coords = ray_position_lightspace.xyz / ray_position_lightspace.w;
		
		// transform to [0,1] range
		proj_coords = proj_coords * 0.5 + 0.5;		
		
		// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
		vec4 closest_depth = texture(depthTex, proj_coords.xy);
		
		float shadow_term = 1.0;
		
		if (proj_coords.z - bias > closest_depth.r) {
			shadow_term = 0.0;
		}
		
		float d = length(ray_position_worldspace.xyz);
		float d_rcp = 1.0/d;
		
		float fog = sample_fog(ray_position_worldspace.xyz);;
		
		light_contribution += fog * tau * (shadow_term * (phi * 0.25 * PI) * d_rcp * d_rcp ) * exp(-d*tau)*exp(-l*tau) * step_size_worldspace;
	
		ray_position_lightview += step_size_lightview * delta_lightview;
		ray_position_worldspace += step_size_worldspace * delta_worldspace;
	}
	
	return min(light_contribution, 1.0);
}

void main() {
	
	float depth = texture(depthTex, uv).r;
	
	//if (depth == 1.0) {
	//	discard; // here it would sample into infinity
	//}
	
	vec3 frag_pos = world_pos_from_depth(depth);
	
	vec3 vol_color = volumetric_lighting_directional(frag_pos, dLights[0]) * dLights[0].intensity.rgb;
	
	FragColor = vec4(vol_color, depth);
}
