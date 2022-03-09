#version 460

in vec3 fNormal;
in vec3 fPosition;
in vec2 fUV;

struct Material
{
	sampler2D albedo;
	sampler2D normal;
    sampler2D metallic;
    sampler2D roughness;
    sampler2D ao;
	samplerCube irradiance;
};

uniform Material material;

out vec4 FragColor;

void main()
{
   FragColor = texture(material.irradiance, fPosition);
}
