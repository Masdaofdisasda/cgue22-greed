#include "Material.h"

// expects path to be "textures/rockground/albedo.jpg"

/// @brief loads five textures from a texture folder
/// @param texPath should be of the form "textures/(Material_1)/albedo.jpg"
/// @param name the name of the material, eg. Material_1
Material::Material(const char* texPath, const char* name) {

	id = name;

	// remove "/albedo.jpg" from path end
	std::string path = texPath;
	size_t start = path.length() - 11;
	path.erase(start, 11);

	// append "../../assests/" to the start of the string
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

/// @brief adds a subfolder to a given path
/// @param texPath is the path to the root folder
/// @param texType is the name of the image file in the root folder
/// @return the path to the image file
const char* Material::append(const char* texPath, char* texType)
{
	char c[100];
	strcpy(c, texPath);

	return strcat(c, texType);
}

/// @brief explicitly deletes every texture in this material
void Material::clear()
{
	glDeleteTextures(1, &albedo);
	glDeleteTextures(1, &normal);
	glDeleteTextures(1, &metal);
	glDeleteTextures(1, &rough);
	glDeleteTextures(1, &ao);
}