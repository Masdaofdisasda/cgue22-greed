#pragma once

#include "Utils.h"
#include "Texture.h"
#include <vector>

#ifdef __GNUC__
# define PACKED_STRUCT __attribute__((packed, aligned(1)))
#else
# define PACKED_STRUCT
#endif

constexpr const uint64_t INVALID_TEXTURE = 0xFFFFFFFF;

enum MaterialFlags
{
	visible = 0x1,
	invisible = 0x2,
};

/// @brief Material describes a material with five textures
/// loads five textures from a path and manages their handles
/// caution: this class is not RAII comliant, every created material
/// needs to be manually deleted by calling clear() 
struct PACKED_STRUCT material final
{
	//std::string name;

	GLuint albedo_ = 0;
	GLuint normal_ = 0;
	GLuint metal_ = 0;
	GLuint rough_ = 0;
	GLuint ao_ = 0;
	GLuint emissive_ = 0;
	GLuint height_ = 0;

	uint64_t albedo64_ = INVALID_TEXTURE;
	uint64_t normal64_ = INVALID_TEXTURE;
	uint64_t metal64_ = INVALID_TEXTURE;
	uint64_t rough64_ = INVALID_TEXTURE;
	uint64_t ao64_ = INVALID_TEXTURE;
	uint64_t emissive64_ = INVALID_TEXTURE;
	uint64_t height64_ = INVALID_TEXTURE;

	uint64_t type = visible;

/// expects path to be "textures/rockground/albedo.jpg"
/// @brief loads five textures from a texture folder
/// @param tex_path should be of the form "textures/(Material_1)/albedo.ktx"
/// @param name of the material, eg. Material_1
	static void create(const char* tex_path, const char* name, material& mat);

/// @brief explicitly deletes every texture in this material
	static void clear(material& mat);
};

static_assert(sizeof(material) % 16 == 0, "material should be padded to 16 bytes");
