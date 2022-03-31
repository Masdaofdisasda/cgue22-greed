#include "Material.h"


Material::Material(const char* texPath) {

albedo = Texture::loadTexture(append(texPath, "/albedo.jpg"));
normal = Texture::loadTexture(append(texPath, "/normal.jpg"));
metal = Texture::loadTexture(append(texPath, "/metal.jpg"));
rough = Texture::loadTexture(append(texPath, "/rough.jpg"));
ao = Texture::loadTexture(append(texPath, "/ao.jpg"));
}


const char* Material::append(const char* texPath, char* texType)
{
	char c[100];
	strcpy(c, texPath);

	return strcat(c, texType);
}