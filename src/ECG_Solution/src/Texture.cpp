#include "Texture.h"


Texture::Texture(const char* texPath, int texUnit)
{
	// generate diffuse texture
	glGenTextures(1, &tex_ID);
	glActiveTexture(GL_TEXTURE0+ texUnit);
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
