#include "Material.h"


Material::Material(const char* texPath, const char* cubePath)
{
	
	albedo.load(append(texPath, "/albedo.jpg"));
	normal.load(append(texPath, "/normal.jpg"));
	metallic.load(append(texPath, "/metal.jpg"));
	roughness.load(append(texPath, "/rough.jpg"));
	std::cout << append(texPath, "/test.jpg") << std::endl;
	ambientocclusion.load(append(texPath, "/ao.jpg"));
	cubemap.loadHDR(cubePath);
}

Material::Material(const char* cubePath)
{
	cubemap.loadHDR(cubePath);
}

const char* Material::append(const char* texPath, char* texType)
{
	char c[100];
	strcpy(c, texPath);

	return strcat(c, texType);
}