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

/// @brief loads a texture from image, used in Material.h
/// @param texPath is the location of an image
/// @return the created texture handle
GLuint Texture::loadTextureTransparent(const char* texPath)
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
	const uint8_t* img = stbi_load(texPath, &w, &h, &comp, STBI_rgb_alpha);

	if (img > 0)
	{
		glTextureStorage2D(handle, 1, GL_RGBA8, w, h);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(handle, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
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

/// @brief loads a 3dlut in .cube format, used for color grading in Renderer
/// code from https://svnte.se/3d-lut
/// @param texPath is the location of the lut
/// @return the created texture handle
GLuint Texture::load3Dlut(const char* texPath)
{
	// Load .CUBE file 
	printf("Loading LUT file %s \n", texPath);
	FILE* file = fopen(texPath, "r");

	if (file == NULL) {
		printf("Could not open file \n");
		return false;
	}

	float* lut_data = nullptr;
	int size = 0;

	// Iterate through lines
	while (true) {
		char line[128];
		fscanf(file, "%128[^\n]\n", line);



		if (strcmp(line, "#LUT size") == 0) {
			// Read LUT size
			fscanf(file, "%s %i\n", &line, &size);
			lut_data = new float[size * size * size * 3];
		}
		else if (strcmp(line, "#LUT data points") == 0) {

			// Read colors
			int row = 0;
			do {
				float r, g, b;
				fscanf(file, "%f %f %f\n", &r, &g, &b);
				lut_data[row * 3 + 0] = r;
				lut_data[row * 3 + 1] = g;
				lut_data[row * 3 + 2] = b;
				row++;
			} while (row < size * size * size);
			break;
		}
	}
	fclose(file);

	// Create texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_3D, texture);

	// Load data to texture
	glTexImage3D(

		GL_TEXTURE_3D,
		0,
		GL_RGB,
		size, size, size,
		0,
		GL_RGB,
		GL_FLOAT,
		lut_data
	);

	// Set sampling parameters
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

	return texture;

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
