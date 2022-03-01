#version 430

in vec3 fColor;
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
	sampler2D tex0;
	sampler2D tex1;
	samplerCube tex2;
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

vec3 Idirectional(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 Ipoint(PositionalLight light, vec3 normal, vec3 fPosition, vec3 viewDir);
vec3 Ispot(SpotLight light, vec3 normal, vec3 fPosition, vec3 viewDir);
vec3 calculateLight();

void main()
{
    if(material.reflection == 1.0f)
    {
    FragColor = texture(material.tex2, fPosition);
    }else
    {
        vec4 light = vec4(calculateLight(),1.0f);

        vec3 I = normalize(fPosition - viewPos);
        vec3 R = reflect(I, normalize(fNormal));
        vec4 refl = vec4(texture(material.tex2, R).xyz, 1.0) * vec4(texture(material.tex0, fUV));
        FragColor = mix(light, refl,  material.reflection) ;
    }
}

vec3 calculateLight() {
	// define an output color value
	vec3 result = vec3(0.0f);
	
    vec3 norm = normalize(fNormal);
	vec3 viewDir = normalize(viewPos - fPosition);

	// add the directional light's contribution to the output
	for(int i = 0; i < dLightCount; i++)
	result += Idirectional(dLights[i], norm, viewDir);

	// do the same for all point lights
	for(int i = 0; i < pLightCount; i++)
  	result += Ipoint(pLights[i], norm, fPosition, viewDir);

	// and add spotlights as well
	for(int i = 0; i < sLightCount; i++)
	result += Ispot(sLights[i], vec3(norm), fPosition, viewDir);

    return result;
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
    vec3 ambient  = texture(material.tex0, fUV).xyz * material.coefficients.x;
    vec3 diffuse  = diff * texture(material.tex0, fUV).xyz * material.coefficients.y;
    vec3 specular = spec * texture(material.tex1, fUV).xyz * material.coefficients.z;
    return light.intensity.xyz * (ambient + diffuse + specular);
}  

// calculate postional lights
vec3 Ipoint(PositionalLight light, vec3 normal, vec3 fPosition, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fPosition);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfway = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfway), 0.0f), material.coefficients.w);

    // attenuation
    float distance    = length(light.position.xyz - fPosition);
    float attenuation = 1.0f / (light.attenuation[0] + light.attenuation[1] * distance + 
  			     light.attenuation[2] * (distance * distance));    

    // combine results
    vec3 ambient  = texture(material.tex0, fUV).xyz * material.coefficients.x;
    vec3 diffuse  = diff * texture(material.tex0, fUV).xyz * material.coefficients.y;
    vec3 specular = spec * texture(material.tex1, fUV).xyz * material.coefficients.z;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return light.intensity.xyz * (ambient + diffuse + specular);
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
    vec3 ambient  = texture(material.tex0, fUV).xyz * material.coefficients.x;
    vec3 diffuse  = diff * texture(material.tex0, fUV).xyz * material.coefficients.y;
    vec3 specular = spec * texture(material.tex1, fUV).xyz * material.coefficients.z;
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    return light.intensity.xyz * (ambient + diffuse + specular);
}