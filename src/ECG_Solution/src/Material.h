#pragma once

#include "Utils.h"
#include "Texture.h"
#include <vector>

/* class for materials
* every material holds a vector of 5 textures and can return handles to them
*/
class Material
{
public:
	Material(const char* texPath);
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

	//helper functions for easier loading
	const char* append(const char* texPath, char* texType);

	void Release()
	{
		// TODO delete textures
	}

};

