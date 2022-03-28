#pragma once

#include "Utils.h"

/* class for basic textures
* holds a unique id, a path for comparison can be a texture or a cubemap
*/
class Texture
{
private:
	GLuint tex_ID = 0;
	GLenum type_ = 0;

	void Release()
	{
		glDeleteTextures(1, &tex_ID);
		tex_ID = 0;
	}

	int getNumMipMapLevels2D(int w, int h);

public:
	Texture();

	Texture(GLenum type, int width, int height, GLenum internalFormat);
	~Texture() { Release(); }

	void load(const char* texPath, int texUnit = 0);

	
	// ensure RAII compliance
	Texture(const Texture&) = delete;

	Texture(Texture&& other)
		: type_(other.type_)
		, tex_ID(other.tex_ID)
	{
		other.type_ = 0;
		other.tex_ID = 0;
	}

	GLuint getHandle() const { return tex_ID; }
};
