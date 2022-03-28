#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


Texture::Texture()
{
	tex_ID = 0;
}

Texture::Texture(GLenum type, int width, int height, GLenum internalFormat)
	: type_(type)
{
	glCreateTextures(type, 1, &tex_ID);
	glTextureParameteri(tex_ID, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(tex_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(tex_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(tex_ID, getNumMipMapLevels2D(width, height), internalFormat, width, height);
}

void Texture::load(const char* texPath, int texUnit)
{
	// generate texture
	glCreateTextures(GL_TEXTURE_2D, 1, &tex_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, comp;
	const uint8_t* img = stbi_load(texPath, &w, &h, &comp, 3);

	if (img > 0)
	{
		glTextureStorage2D(tex_ID, 1, GL_RGB8, w, h);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(tex_ID, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img);
		glBindTextures(0, 1, &tex_ID);

	}
	else
	{
		std::cout << "could not load texture" << texPath << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

}

int Texture::getNumMipMapLevels2D(int w, int h)
{
	int levels = 1;
	while ((w | h) >> levels)
		levels += 1;
	return levels;
}
