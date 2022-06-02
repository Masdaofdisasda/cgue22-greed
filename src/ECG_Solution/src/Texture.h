#pragma once

#include "gli/gli.hpp"
#include "Utils.h"
#include <vector>

struct image_data
{
	uint8_t* data;
	int w, h, comp;
};

/// @brief Texture is an OpenGL sampler2D or sampler3D aka Texture or Cubemap
/// it can create textures from images or pset them up for framebuffers
class Texture
{
private:
	GLuint tex_id_ = 0;
	GLenum type_ = 0;
	static GLuint defaults_[7];
	static uint64_t defaults64_[7];

	void release()
	{
		glDeleteTextures(1, &tex_id_);
		tex_id_ = 0;
	}

	/// @brief calculates mimap level for framebuffer textures
	/// @param w width of the texture
	/// @param h height of the texture
	/// @return the number of mipmap levels needed
	static int get_num_mip_map_levels_2d(int w, int h);

	/**
	 * \brief appends file names to folder location
	 * \param tex_path folder
	 * \param tex_type file name
	 * \return full file path
	 */
	static std::string append(const char* tex_path, const char* tex_type);

public:
	/**
	 * \brief create an empty texture (used for as frame buffer attachment)
	 * \param type of the texture eg 2D_TEXTURE
	 * \param width of the texture (same as framebuffer)
	 * \param height of the textuer (same as framebuffer)
	 * \param internal_format is the color or depth format
	 */
	Texture(GLenum type, int width, int height, GLenum internal_format);
	~Texture() { release(); }

	/**
	 * \brief loads a single texture from a ktx file
	 * \param tex_path location of the file
	 * \return GL handle
	 */
	static GLuint load_texture(const char* tex_path);

	/**
	 * \brief loads a full material from ktx files
	 * \param tex_path location of the material
	 * \param handles GL handles for the textures after function call
	 * \param bindless Gl handles for the bindless textures after fucntion call
	 */
	static void load_texture_mt(const char* tex_path, GLuint handles[], uint64_t bindless[]);

	/**
	 * \brief loads a 3dlut in .cube format, used for color grading in Renderer
	 *code from https://svnte.se/3d-lut
	 * \param tex_path is the location of the lut
	 * \return the created texture handle
	 */
	static GLuint load_3dlut(const char* tex_path);

	/**
	 * \brief only for testing
	 * \param tex_path 
	 * \param img 
	 */
	static void stbi_load_single(const std::string& tex_path, image_data* img);

	/**
	 * \brief generates a SSAO filter kernel texture
	 * from OpenGL 4 Shading Language Cookbook
	 * \return handle to the texture
	 */
	static GLuint get_ssao_kernel();

	static GLuint get_3D_noise(int size, float base_freq);

	/**
	 * \brief deletes textures
	 * \param handles of texture
	 * \param bindless handles of texture
	 */
	static void destory_texture_mt(GLuint handles[], uint64_t bindless[]);

	static void destroy_defaults();

	
	// ensure RAII compliance
	Texture(const Texture&) = delete;

	Texture(Texture&& other)
		: type_(other.type_)
		, tex_id_(other.tex_id_)
	{
		other.type_ = 0;
		other.tex_id_ = 0;
	}

	GLuint get_handle() const { return tex_id_; }
};
