#version 460

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 fPosition;

layout(std140, binding = 0) uniform PerFrameData
{
	vec4 viewPos;
	mat4 ViewProj;
	mat4 ViewProjSkybox;
};

uniform samplerCube environment;

void main()
{
   vec3 color = textureLod(environment, vec3(fPosition), 0.0).xyz;

   // HDR tonemap and gamma correct
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
	{
        BrightColor = vec4(color, 1.0);
    }
    else
	{
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    FragColor = vec4(color, 1.0);
}
