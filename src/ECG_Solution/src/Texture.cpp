#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

/// @brief create a new texture, used in Framebuffer.h
/// @param type GL Texture type, eg GL_TEXTURE_2D
/// @param width of the texture (same as framebuffer)
/// @param height of the texture (same as framebuffer)
/// @param internalFormat is the color or depth format
Texture::Texture(GLenum type, int width, int height, GLenum internalFormat)
	: type_(type)
{
	glCreateTextures(type, 1, &tex_ID);
	glTextureParameteri(tex_ID, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(tex_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(tex_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(tex_ID, getNumMipMapLevels2D(width, height), internalFormat, width, height);
}

/// @brief loads a texture from image, used in Material.h
/// @param texPath is the location of an image
/// @return the created texture handle
GLuint Texture::loadTexture(const char* texPath)
{
	GLuint handle = 0;
	// generate texture
	glCreateTextures(GL_TEXTURE_2D, 1, &handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int w, h, comp;
	const uint8_t* img = stbi_load(texPath, &w, &h, &comp, 3);

	if (img > 0)
	{
		glTextureStorage2D(handle, 1, GL_RGB8, w, h);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(handle, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img);
		glBindTextures(0, 1, &handle);
		delete img;
	}
	else
	{
		std::cout << "could not load texture" << texPath << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return handle;
}

/// @brief calculates mimap level for framebuffer textures
/// @param w width of the texture
/// @param h height of the texture
/// @return the number of mipmap levles
int Texture::getNumMipMapLevels2D(int w, int h)
{
	int levels = 1;
	while ((w | h) >> levels)
		levels += 1;
	return levels;
}
