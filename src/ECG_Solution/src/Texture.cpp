#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <thread>

GLuint Texture::defaults_[6] = { 0,0,0,0,0,0 };
uint64_t Texture::defaults64_[6] = { 0,0,0,0,0,0 };

/// @brief create a new texture, used in Framebuffer.h
/// @param type GL Texture type, eg GL_TEXTURE_2D
/// @param width of the texture (same as framebuffer)
/// @param height of the texture (same as framebuffer)
/// @param internal_format is the color or depth format
Texture::Texture(const GLenum type, const int width, const int height, const GLenum internal_format)
	: type_(type)
{
	glCreateTextures(type, 1, &tex_id_);
	glTextureParameteri(tex_id_, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(tex_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(tex_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(tex_id_, get_num_mip_map_levels_2d(width, height), internal_format, width, height);
}
/// @brief loads a texture from image, used in Material.h
/// @param tex_path is the location of an image
/// @return the created texture handle
GLuint Texture::load_texture(const char* tex_path)
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
	const uint8_t* img = stbi_load(tex_path, &w, &h, &comp, 3);

	if (img > nullptr)
	{
		glTextureStorage2D(handle, 1, GL_RGB8, w, h);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(handle, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img);
		glBindTextures(0, 1, &handle);
		delete img;
	}
	else
	{
		std::cout << "could not load texture" << tex_path << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return handle;
}

/// @brief load 6 pbr textures using multiple threads
/// @param tex_path folder location of the material
/// @param handles target containing the texture handles after call
void Texture::load_texture_mt(const char* tex_path, GLuint handles[], uint64_t bindless[])
{
	
	stbiData img_data[7]; std::thread workers[7];

	std::string albedo = append(tex_path, "/albedo.png");
	workers[0] = std::thread (Texture::stbi_load_single, albedo, &img_data[0]);
	std::string normal = append(tex_path, "/normal.png");
	workers[1] = std::thread (Texture::stbi_load_single, normal, &img_data[1]);
	std::string metal = append(tex_path, "/metal.png");
	workers[2] = std::thread (Texture::stbi_load_single, metal, &img_data[2]);
	std::string rough = append(tex_path, "/rough.png");
	workers[3] = std::thread (Texture::stbi_load_single, rough, &img_data[3]);
	std::string ao = append(tex_path, "/ao.png");
	workers[4] = std::thread(Texture::stbi_load_single, ao, &img_data[4]);
	std::string emissive = append(tex_path, "/emissive.png");
	workers[5] = std::thread(Texture::stbi_load_single, emissive, &img_data[5]);
	std::string height = append(tex_path, "/emissive.png");
	workers[6] = std::thread(Texture::stbi_load_single, height, &img_data[6]);

	for (size_t i = 0; i < 7; i++)
	{
		workers[i].join();
		
		if (img_data[i].data > nullptr)
		{
			const int mipMapLevel = get_num_mip_map_levels_2d(img_data[i].w, img_data[i].h);
			glCreateTextures(GL_TEXTURE_2D, 1, &handles[i]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glTextureStorage2D(handles[i], mipMapLevel, GL_RGBA8, img_data[i].w, img_data[i].h);
			glTextureSubImage2D(handles[i], 0, 0, 0, img_data[i].w, img_data[i].h, GL_RGBA, GL_UNSIGNED_BYTE, img_data[i].data);
			glGenerateTextureMipmap(handles[i]);
			glTextureParameteri(handles[i], GL_TEXTURE_MAX_LEVEL, mipMapLevel - 1);
			glTextureParameteri(handles[i], GL_TEXTURE_MAX_ANISOTROPY, 16);
			bindless[i] = glGetTextureHandleARB(handles[i]);
			glMakeTextureHandleResidentARB(bindless[i]);
			delete img_data[i].data;
		}
		else
		{
#ifdef _DEBUG
			std::cout << "could not load texture nr " <<i << " from " << tex_path << "\n"<<"using fallback...\n";
#endif
			if (defaults_[0] == 0)
			{
				load_texture_mt("../../assets/textures/default", defaults_, defaults64_);
			}
			handles[i] = defaults_[i];
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// @brief loads a texture from image, used in Material.h
/// @param tex_path is the location of an image
/// @return the created texture handle
GLuint Texture::load_texture_transparent(const char* tex_path)
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
	const uint8_t* img = stbi_load(tex_path, &w, &h, &comp, STBI_rgb_alpha);

	if (img > nullptr)
	{
		glTextureStorage2D(handle, 1, GL_RGBA8, w, h);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(handle, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glBindTextures(0, 1, &handle);
		delete img;
	}
	else
	{
		std::cout << "could not load texture" << tex_path << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return handle;
}

/// @brief loads a 3dlut in .cube format, used for color grading in Renderer
/// code from https://svnte.se/3d-lut
/// @param tex_path is the location of the lut
/// @return the created texture handle
GLuint Texture::load_3dlut(const char* tex_path)
{
	// Load .CUBE file 
	printf("Loading LUT file %s \n", tex_path);
	FILE* file = fopen(tex_path, "r");

	if (file == nullptr) {
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
int Texture::get_num_mip_map_levels_2d(const int w, const int h)
{
	int levels = 1;
	while ((w | h) >> levels)
		levels += 1;
	return levels;
}

void Texture::stbi_load_single(const std::string& tex_path, stbiData* img)
{
	stbi_set_flip_vertically_on_load(true);

	img->data = stbi_load(tex_path.c_str(), &img->w, &img->h, &img->comp, STBI_rgb_alpha);
}

/// @brief adds a subfolder to a given path
/// @param tex_path is the path to the root folder
/// @param tex_type is the name of the image file in the root folder
/// @return the path to the image file
std::string Texture::append(const char* tex_path, const char* tex_type)
{
	char c[100];
	strcpy(c, tex_path);
	std::string result = strcat(c, tex_type);
	return result;
}