#include "Cubemap.h"

Cubemap::Cubemap(const char* texPath)
{
	// generate cube map
	glGenTextures(1, &cube_ID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_ID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	char c[100];
	strcpy(c, texPath);

	const char* location = strcat(c, "/negx.dds");
	DDSImage imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, texPath);
	location = strcat(c, "/negy.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, texPath);
	location = strcat(c, "/negz.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, texPath);
	location = strcat(c, "/posx.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, texPath);
	location = strcat(c, "/posy.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	strcpy(c, texPath);
	location = strcat(c, "/posz.dds");
	imgMap = loadDDS(location);
	glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, imgMap.format, imgMap.width, imgMap.height, 0, imgMap.size, imgMap.data);

	// debugging black cubemap
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "ERROR" << std::endl;
		std::cout << err << std::endl;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}