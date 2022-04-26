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

	// append "../../assets/" to the start of the string
	char* file = new char[path.length() + 1];
	strcpy(file, path.c_str());
	char c[100];
	strcpy(c, "../../assets/");
	strcat(c, file);

	// load textures multithreaded
	GLuint handles[5];
	Texture::loadTextureMT(c, handles);
	albedo = handles[0];
	normal = handles[1];
	metal = handles[2];
	rough = handles[3];
	ao = handles[4];
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