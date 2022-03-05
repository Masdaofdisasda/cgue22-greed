#version 460

in vec3 fNormal;
in vec3 fPosition;
in vec2 fUV;

// light sources
// use vec4 instead vec3 for GLSL std140 data layout standard
struct DirectionalLight
{
	vec4 direction;

	vec4 intensity;
};

struct PositionalLight
{
	vec4 position;
	vec4 attenuation;
    
	vec4 intensity;
};

struct SpotLight
{
	vec4 position;
	vec4 direction;
	vec4 Angles;
	vec4 attenuation;
    
	vec4 intensity;
};

struct Material
{
	sampler2D albedo;
	sampler2D specular;
	samplerCube irradiance;
    vec4 coefficients;
    float reflection;
};

// light source data in uniform blocks
// MAXLIGHTS gets replaced at runtime
layout (std140, binding = 0) uniform dLightUBlock {
 DirectionalLight dLights [ dMAXLIGHTS ];
};
uniform uint dLightCount ;

layout (std140, binding = 1) uniform pLightUBlock {
 PositionalLight pLights [ pMAXLIGHTS ];
};
uniform uint pLightCount ;

layout (std140, binding = 2) uniform sLightUBlock {
 SpotLight sLights [ sMAXLIGHTS ];
};
uniform uint sLightCount ;

uniform Material material;

uniform vec3 viewPos;

out vec4 FragColor;

//todo
vec3 albedo = pow(texture(material.albedo, fUV).rgb, vec3(2.2));
float metallic = material.coefficients.x;
float roughness = material.coefficients.y;
float ao = material.coefficients.z;

const float PI = 3.14159265359;

vec3 Idirectional(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 Ipoint(PositionalLight light, vec3 N, vec3 fPosition, vec3 V, vec3 F0);
vec3 Ispot(SpotLight light, vec3 normal, vec3 fPosition, vec3 viewDir);
vec3 calculateLight();

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

void main()
{
    FragColor = vec4(calculateLight(),1.0f);
}

vec3 calculateLight() {
	
    vec3 N = normalize(fNormal);
	vec3 V = normalize(viewPos - fPosition);
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
	//result += Idirectional(dLights[i], N, V);

	// do the same for all point lights
	for(int i = 0; i < pLightCount; i++)
  	Lo += Ipoint(pLights[i], N, fPosition, V, F0);

	// and add spotlights as well
	for(int i = 0; i < sLightCount; i++)
	//result += Ispot(sLights[i], vec3(N), fPosition, V);

    // ambient lighting (we now use IBL as the ambient term)
    F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(material.irradiance, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    vec3 ambient = (kD * diffuse) * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    return color;
}

// calculate directional lights
vec3 Idirectional(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(vec3(-light.direction));

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfway = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfway), 0.0f), material.coefficients.w);

    // combine results
    vec3 ambient  = texture(material.albedo, fUV).xyz * material.coefficients.x;
    vec3 diffuse  = diff * texture(material.albedo, fUV).xyz * material.coefficients.y;
    vec3 specular = spec * texture(material.specular, fUV).xyz * material.coefficients.z;
    return light.intensity.xyz * (ambient + diffuse + specular);
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
        vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);        
        
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

// calculate spot lights
vec3 Ispot(SpotLight light, vec3 normal, vec3 fPosition, vec3 viewDir)
{
    vec3 lightDir = normalize(vec3(light.position) - fPosition);
    

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfway = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfway), 0.0f), material.coefficients.w);

    // attenuation
    float distance    = length(vec3(light.position) - fPosition);
    float attenuation = 1.0f / (light.attenuation[0] + light.attenuation[1] * distance + 
  			     light.attenuation[2] * (distance * distance));    

    // cone light (x is outer angle, y is inner angle)
    float theta     = dot(lightDir, normalize(vec3(-light.direction)));
    float epsilon   = light.Angles.y - light.Angles.x;
    float intensity = smoothstep(0.0, 1.0, (theta - light.Angles.x) / epsilon);

    // combine results
    vec3 ambient  = texture(material.albedo, fUV).xyz * material.coefficients.x;
    vec3 diffuse  = diff * texture(material.albedo, fUV).xyz * material.coefficients.y;
    vec3 specular = spec * texture(material.specular, fUV).xyz * material.coefficients.z;
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    return light.intensity.xyz * (ambient + diffuse + specular);
}