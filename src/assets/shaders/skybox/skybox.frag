#version 460

in vec3 fNormal;
in vec3 fPosition;
in vec2 fUV;

struct Material
{
	sampler2D albedo;
	sampler2D specular;
	samplerCube irradiance;
    vec4 coefficients;
    float reflection;
};

uniform Material material;

out vec4 FragColor;

void main()
{
   FragColor = texture(material.irradiance, fPosition);
}
