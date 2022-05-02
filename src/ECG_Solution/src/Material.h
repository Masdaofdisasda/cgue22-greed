#pragma once

#include "Utils.h"
#include "Texture.h"
#include <vector>

/// @brief Material describes a material with five textures
/// loads five textures from a path and manages their handles
/// caution: this class is not RAII comliant, every created texure
/// needs to be manually deleted by calling clear() 
class Material
{
public:
	Material(const char* tex_path, const char* name);
	~Material() { release(); };
	std::string name;

	GLuint get_albedo() const { return albedo_; }
	GLuint get_normal_map() const { return normal_; }
	GLuint get_metallic() const { return metal_; }
	GLuint get_roughness() const { return rough_; }
	GLuint get_ao_map() const { return ao_; }
	GLuint get_emissive() const { return emissive_; }

	void clear() const;

private:

	GLuint albedo_ = 0;
	GLuint normal_ = 0;
	GLuint metal_ = 0;
	GLuint rough_ = 0;
	GLuint ao_ = 0;
	GLuint emissive_ = 0;


	void release()
	{
		// do not delete textures here
	}

};

