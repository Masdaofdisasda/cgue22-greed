#pragma once

#include "Utils.h"

/// @brief Texture is an OpenGL sampler2D or sampler3D aka Texture or Cubemap
/// it can create textures from images or pset them up for framebuffers
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
	Texture(GLenum type, int width, int height, GLenum internalFormat);
	~Texture() { Release(); }

	static GLuint loadTexture(const char* texPath);
	static GLuint loadTextureTransparent(const char* texPath);
	static GLuint load3Dlut(const char* texPath);

	
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
