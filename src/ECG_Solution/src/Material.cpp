#include "Material.h"

// expects path to be "textures/rockground/albedo.jpg"

/// @brief loads five textures from a texture folder
/// @param tex_path should be of the form "textures/(Material_1)/albedo.jpg"
/// @param name the name of the material, eg. Material_1
Material::Material(const char* tex_path, const char* name) {

	this->name = name;

	// remove "/albedo.jpg" from path end
	std::string path = tex_path;
	const size_t start = path.length() - 11;
	path.erase(start, 11);

	// append "../../assets/" to the start of the string
	const auto file = new char[path.length() + 1];
	strcpy(file, path.c_str());
	char c[100];
	strcpy(c, "../../assets/");
	strcat(c, file);

	// load textures multi-threaded
	GLuint handles[6];
	Texture::load_texture_mt(c, handles);
	albedo_ = handles[0];
	normal_ = handles[1];
	metal_ = handles[2];
	rough_ = handles[3];
	ao_ = handles[4];
	emissive_ = handles[5];
}

/// @brief explicitly deletes every texture in this material
void Material::clear() const
{
	
	glDeleteTextures(1, &albedo_);
	glDeleteTextures(1, &normal_);
	glDeleteTextures(1, &metal_);
	glDeleteTextures(1, &rough_);
	glDeleteTextures(1, &ao_);
	glDeleteTextures(1, &emissive_);
}
