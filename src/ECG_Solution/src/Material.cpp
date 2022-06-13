#include "Material.h"


void material::create(const char* tex_path, const char* name, material& mat) {
	
	// remove "/albedo.jpg" from path end
	std::string path = tex_path;
	const size_t start = path.length() - 11;
	path.erase(start, 11);

	// append "../assets/" to the start of the string
	const auto file = new char[path.length() + 1];
	strcpy(file, path.c_str());
	char c[100];
	strcpy(c, "../assets/");
	strcat(c, file);

	// load textures
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

void material::clear(material& mat)
{

	GLuint handles[7];
	uint64_t bindless[7];

	handles[0] = mat.albedo_;
	handles[1] = mat.normal_;
	handles[2] = mat.metal_;
	handles[3] = mat.rough_;
	handles[4] = mat.ao_;
	handles[5] = mat.emissive_;
	handles[6] = mat.height_;

	bindless[0] = mat.albedo64_;
	bindless[1] = mat.normal64_;
	bindless[2] = mat.metal64_;
	bindless[3] = mat.rough64_;
	bindless[4] = mat.ao64_;
	bindless[5] = mat.emissive64_;
	bindless[6] = mat.height64_;

	Texture::destory_texture_mt(handles, bindless);
}

