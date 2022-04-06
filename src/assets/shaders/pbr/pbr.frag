#version 460

// in and out variables
layout (location=0) out vec4 out_FragColor;

in vec3 fNormal;
in vec3 fPosition;
in vec2 fUV;

// light sources
struct DirectionalLight
{
	vec4 direction;

	vec4 intensity;
};

layout (std140, binding = 1) uniform dLightUBlock {
 DirectionalLight dLights [ dMAXLIGHTS ]; // xMAXLIGHTS gets replaced at runtime
};
uniform uint dLightCount ;

struct PositionalLight
{
	vec4 position;
    vec4 intensity;
};

layout (std140, binding = 2) uniform pLightUBlock {
 PositionalLight pLights [ pMAXLIGHTS ];
};
uniform uint pLightCount ;

// model textures and ibl cubemaps
layout (binding = 0) uniform sampler2D albedoTex;
layout (binding = 1) uniform sampler2D normalTex;
layout (binding = 2) uniform sampler2D metallicTex;
layout (binding = 3) uniform sampler2D roughnessTex;
layout (binding = 4) uniform sampler2D aoTex;

layout (binding = 5) uniform samplerCube irradianceTex;
layout (binding = 6) uniform samplerCube prefilterTex;
layout (binding = 7) uniform sampler2D brdfLutTex;


// constant per frame data
layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 ViewProjSkybox;
	vec4 bloom;
	vec4 deltaTime;
    vec4 normalMap;
};

// Global variables
vec3 albedo = pow(texture(albedoTex, fUV).rgb, vec3(2.2));
float metallic = texture(metallicTex, fUV).r;
float roughness = texture(roughnessTex, fUV).r;
float ao = texture(aoTex,fUV).r;

const float PI = 3.14159265359;


// helper functions

// http://www.thetenthplanet.de/archives/1180
mat3 cotangentFrame( vec3 N, vec3 p, vec2 uv )
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );

	// solve the linear system
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );

	// calculate handedness of the resulting cotangent frame
	float w = (dot(cross(N, T), B) < 0.0) ? -1.0 : 1.0;

	// adjust tangent if needed
	T = T * w;

	return mat3( T * invmax, B * invmax, N );
}

vec3 perturbNormal(vec3 n, vec3 v, vec3 normalSample, vec2 uv)
{
	vec3 map = normalize( 2.0 * normalSample - vec3(1.0) );
	mat3 TBN = cotangentFrame(n, v, uv);
	return normalize(TBN * map);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   


// calculate directional lights
vec3 Idirectional(DirectionalLight light, vec3 N, vec3 fPosition, vec3 V, vec3 F0)
{
        // calculate per-light radiance
        vec3 L = normalize(vec3(light.direction) );
        vec3 H = normalize(V + L);
        vec3 radiance = light.intensity.xyz;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G = GeometrySmith(N, V, L, roughness);    
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        return (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    
}  

// calculate postional lights
vec3 Ipoint(PositionalLight light, vec3 N, vec3 fPosition, vec3 V, vec3 F0)
{
        // calculate per-light radiance
        vec3 L = normalize(vec3(light.position) - fPosition);
        vec3 H = normalize(V + L);
        float distance = length(vec3(light.position) - fPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.intensity.xyz * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G = GeometrySmith(N, V, L, roughness);    
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        return (kD * albedo / PI + specular) * radiance * NdotL;
    
}

vec3 calculateLight() {
	
    vec3 N = normalize(fNormal);
	vec3 V = normalize(vec3(viewPos) - fPosition);
    if (normalMap.x > 0.0f) N = perturbNormal(normalize(fNormal), V, texture(normalTex, fUV).xyz, fUV);
    vec3 R = reflect(-V, N);
    
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 F; // needs to be at the top, otherwise it will be "undefined"

    // reflectance equation
    vec3 Lo = vec3(0.0);

	// add the directional light's contribution to the output
	for(int i = 0; i < dLightCount; i++)
	Lo += Idirectional(dLights[i], N, fPosition, V, F0);

	// do the same for all point lights
	for(int i = 0; i < pLightCount; i++)
  	Lo += Ipoint(pLights[i], N, fPosition, V, F0);

    // ambient lighting (we now use IBL as the ambient term)
    F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceTex, N).rgb;
    vec3 diffuse      = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterTex, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLutTex, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;
    
    return color;
}


void main()
{
    vec3 light = calculateLight();
    // Tonemapping is done in CombineHDR.frag
    out_FragColor = vec4(light, 1.0);
    //out_FragColor = vec4(N,1.0);
}