#include "material.h"

// expects path to be "textures/rockground/albedo.jpg"

/// @brief loads five textures from a texture folder
/// @param tex_path should be of the form "textures/(Material_1)/albedo.jpg"
/// @param name the name of the material, eg. Material_1
void material::create(const char* tex_path, const char* name, material& mat) {
	
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
	GLuint handles[7];
	uint64_t bindless[7];
	Texture::load_texture_mt(c, handles, bindless);

	mat.albedo_ = handles[0];
	mat.normal_ = handles[1];
	mat.metal_ = handles[2];
	mat.rough_ = handles[3];
	mat.ao_ = handles[4];
	mat.emissive_ = handles[5];
	mat.height_ = handles[6];

	mat.albedo64_ = bindless[0];
	mat.normal64_ = bindless[1];
	mat.metal64_ = bindless[2];
	mat.rough64_ = bindless[3];
	mat.ao64_ = bindless[4];
	mat.emissive64_ = bindless[5];
	mat.height64_ = bindless[6];
}

/// @brief explicitly deletes every texture in this material
void material::clear(material& mat)
{
	glMakeTextureHandleNonResidentARB(mat.albedo64_);
	glDeleteTextures(1, &mat.albedo_);
	glMakeTextureHandleNonResidentARB(mat.normal64_);
	glDeleteTextures(1, &mat.normal_);
	glMakeTextureHandleNonResidentARB(mat.metal64_);
	glDeleteTextures(1, &mat.metal_);
	glMakeTextureHandleNonResidentARB(mat.rough64_);
	glDeleteTextures(1, &mat.rough_);
	glMakeTextureHandleNonResidentARB(mat.ao64_);
	glDeleteTextures(1, &mat.ao_);
	glMakeTextureHandleNonResidentARB(mat.emissive64_);
	glDeleteTextures(1, &mat.emissive_);
	glMakeTextureHandleNonResidentARB(mat.height64_);
	glDeleteTextures(1, &mat.height_);
}

