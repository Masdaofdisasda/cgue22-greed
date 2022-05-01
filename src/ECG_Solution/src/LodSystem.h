#pragma once
#include "LevelStructs.h"
#include "Utils.h"
#include <glm\glm.hpp>

class lod_system
{
public:

	static float near_plane;
	static glm::vec4 view_pos;
	static glm::vec4 view_dir;

	/**
	 * \brief selects a LOD based on the projected area of an estimated bounding sphere of a mesh
	 * formula from : Real-Time Rendering, p862
	 * \param lods number of lod meshes to select from
	 * \param aabb the AABB bounds of the mesh
	 * \return a number between 0 and lods-1
	*/
	static uint32_t decide_lod(int32_t lods, bounding_box aabb);
};
