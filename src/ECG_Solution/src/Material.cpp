#include "Material.h"


Material::Material(const char* texPath)
{
	textures.emplace_back(Texture());
	textures.emplace_back(Texture());
	textures.emplace_back(Texture());
	textures.emplace_back(Texture());
	textures.emplace_back(Texture());

	textures[0].load(append(texPath, "/albedo.jpg"));
	textures[1].load(append(texPath, "/normal.jpg"));
	textures[2].load(append(texPath, "/metal.jpg"));
	textures[3].load(append(texPath, "/rough.jpg"));
	textures[4].load(append(texPath, "/ao.jpg"));
}


const char* Material::append(const char* texPath, char* texType)
{
	char c[100];
	strcpy(c, texPath);

	return strcat(c, texType);
}