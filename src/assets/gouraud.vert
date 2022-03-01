#version 430

// VAO Buffer data
layout (location = 0) in vec3 vPosition; // vertex position
layout (location = 1) in vec3 vColor; // vertex color
layout (location = 2) in vec3 vNormal; // vertex normal

// light sources
// use vec4 instead vec3 for GLSL std140 data layout standard
struct DirectionalLight
{
	vec4 direction;

	vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PositionalLight
{
	vec4 position;
	vec4 attenuation;

	vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct SpotLight
{
	vec4 position;
	vec4 direction;
	vec4 Angles;
	vec4 attenuation;

	vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 shininess;
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

// camera matrix
uniform mat4 model;
uniform mat4 viewProject;


out vec4 fColor;

vec3 Idirectional(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 Ipoint(PositionalLight light, vec3 normal, vec3 fPosition, vec3 viewDir);
vec3 Ispot(SpotLight light, vec3 normal, vec3 fPosition, vec3 viewDir);
vec3 calculateLight(vec3 fNormal, vec3 viewPos, vec3 fPosition);

void main()
{
	gl_Position = viewProject * model * vec4(vPosition, 1.0);
	vec3 fPosition = vec3(model * vec4(vPosition, 1.0));

	vec3 fNormal = mat3(transpose(inverse(model))) * vNormal;

     
    vec3 light = calculateLight(fNormal, viewPos, fPosition);
    
	fColor = vec4(light*vColor, 1.0f); 
}

vec3 calculateLight(vec3 fNormal,vec3 viewPos, vec3 fPosition) {
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
    float spec = pow(max(dot(normal, halfway), 0.0f), material.shininess.x);

    // combine results
    vec3 ambient  = material.ambient.xyz * light.ambient.xyz;
    vec3 diffuse  = material.diffuse.xyz * light.diffuse.xyz  * diff;
    vec3 specular = material.specular.xyz * light.specular.xyz * spec;
    return (ambient + diffuse + specular);
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
    float spec = pow(max(dot(normal, halfway), 0.0f), material.shininess.x);

    // attenuation
    float distance    = length(light.position.xyz - fPosition);
    float attenuation = 1.0f / (light.attenuation[0] + light.attenuation[1] * distance + 
  			     light.attenuation[2] * (distance * distance));    

    // combine results
    vec3 ambient  = material.ambient.xyz * light.ambient.xyz;
    vec3 diffuse  = material.diffuse.xyz * light.diffuse.xyz  * diff;
    vec3 specular = material.specular.xyz * light.specular.xyz * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
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
    float spec = pow(max(dot(normal, halfway), 0.0f), material.shininess.x);

    // attenuation
    float distance    = length(vec3(light.position) - fPosition);
    float attenuation = 1.0f / (light.attenuation[0] + light.attenuation[1] * distance + 
  			     light.attenuation[2] * (distance * distance));    

    // cone light (x is outer angle, y is inner angle)
    float theta     = dot(lightDir, normalize(vec3(-light.direction)));
    float epsilon   = light.Angles.y - light.Angles.x;
    float intensity = smoothstep(0.0, 1.0, (theta - light.Angles.x) / epsilon);

    // combine results
    vec3 ambient  = material.ambient.xyz * light.ambient.xyz;
    vec3 diffuse  = material.diffuse.xyz * light.diffuse.xyz  * diff;
    vec3 specular = material.specular.xyz * light.specular.xyz * spec;
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}