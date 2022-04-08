#include "Material.h"

// expects path to be "assets/textures/name"
Material::Material(const char* texPath) {

	albedo = Texture::loadTexture(append(texPath, "/albedo.jpg"));
	normal = Texture::loadTexture(append(texPath, "/normal.jpg"));
	metal = Texture::loadTexture(append(texPath, "/metal.jpg"));
	rough = Texture::loadTexture(append(texPath, "/rough.jpg"));
	ao = Texture::loadTexture(append(texPath, "/ao.jpg"));
}

// expects path to be "textures/rockground/albedo.jpg"
Material::Material(const char* texPath, const char* name) {

	id = name;

	// remove "/albedo.jpg" from path end
	std::string path = texPath;
	size_t start = path.length() - 11;
	path.erase(start, 11);

	// append "assests/" to the start of the string
	char* file = new char[path.length() + 1];
	strcpy(file, path.c_str());
	char c[100];
	strcpy(c, "../../assets/");
	strcat(c, file);

	albedo = Texture::loadTexture(append(c, "/albedo.jpg"));
	normal = Texture::loadTexture(append(c, "/normal.jpg"));
	metal = Texture::loadTexture(append(c, "/metal.jpg"));
	rough = Texture::loadTexture(append(c, "/rough.jpg"));
	ao = Texture::loadTexture(append(c, "/ao.jpg"));
}


const char* Material::append(const char* texPath, char* texType)
{
	char c[100];
	strcpy(c, texPath);

	return strcat(c, texType);
}

void Material::clear()
{
	glDeleteTextures(1, &albedo);
	glDeleteTextures(1, &normal);
	glDeleteTextures(1, &metal);
	glDeleteTextures(1, &rough);
	glDeleteTextures(1, &ao);
}