#version 460

in vec3 fNormal;
in vec3 fPosition;
in vec2 fUV;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 ViewProjSkybox;
};

uniform samplerCube environment;

out vec4 FragColor;

void main()
{
   vec3 color = textureLod(environment, vec3(viewPos), 0.0).xyz;

   // HDR tonemap and gamma correct
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
