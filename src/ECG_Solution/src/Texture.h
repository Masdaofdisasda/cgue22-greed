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
	GLuint tex_id_ = 0;
	GLenum type_ = 0;
	static GLuint defaults_[7];
	static uint64_t defaults64_[7];

	void release()
	{
		glDeleteTextures(1, &tex_id_);
		tex_id_ = 0;
	}

	static int get_num_mip_map_levels_2d(int w, int h);
	static std::string append(const char* tex_path, const char* tex_type);

public:
	Texture(GLenum type, int width, int height, GLenum internal_format);
	~Texture() { release(); }

	static GLuint load_texture(const char* tex_path);
	static void load_texture_mt(const char* tex_path, GLuint handles[], uint64_t bindless[]);
	static GLuint load_texture_transparent(const char* tex_path);
	static GLuint load_3dlut(const char* tex_path);
	static void stbi_load_single(const std::string& tex_path, stbiData* img);

	
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
