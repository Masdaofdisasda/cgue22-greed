#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


Texture::Texture()
{
	path = "";

}

void Texture::load(const char* texPath, int texUnit)
{
	path = texPath;
	
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

// may be needed if we use dds textures, ignore for now
void Texture::loadCompressedTex(const char* texPath, int texUnit)
{
	// generate texture
	glGenTextures(1, &tex_ID);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, tex_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	DDSImage imgTex = loadDDS(texPath);

	if (imgTex.size > 0)
	{
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, imgTex.format, imgTex.width, imgTex.height, 0, imgTex.size, imgTex.data);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		std::cout << "could not load texture" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

}
