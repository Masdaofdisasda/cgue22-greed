#pragma once

#include "Utils.h"
#include <vector>

struct stbiData
{
	uint8_t* data;
	int w, h, comp;
};

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
	static std::string append(const char* texPath, char* texType);

public:
	Texture(GLenum type, int width, int height, GLenum internalFormat);
	~Texture() { Release(); }

	static GLuint loadTexture(const char* texPath);
	static void loadTextureMT(const char* texPath, GLuint handles[]);
	static GLuint loadTextureTransparent(const char* texPath);
	static GLuint load3Dlut(const char* texPath);
	static void stbiLoad(std::string texPath, stbiData* img);

	
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
