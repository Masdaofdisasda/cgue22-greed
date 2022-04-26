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
	Material(const char* texPath, const char* name);
	~Material() { Release(); };

	GLuint getAlbedo() const { return albedo; }
	GLuint getNormalmap() const { return normal; }
	GLuint getMetallic() const { return metal; }
	GLuint getRoughness() const { return rough; }
	GLuint getAOmap() const { return ao; }

	void clear();

private:
	std::string id;

	GLuint albedo = 0;
	GLuint normal = 0;
	GLuint metal = 0;
	GLuint rough = 0;
	GLuint ao = 0;


	void Release()
	{
		// do not delete textures here
	}

};

