#include "LodSystem.h"

float lod_system::near_plane = 0.1f;
glm::vec4 lod_system::view_dir = glm::vec4(0, 0, -1, 1);
glm::vec4 lod_system::view_pos = glm::vec4(0);

uint32_t lod_system::decide_lod(int32_t lods, const bounding_box aabb)
{
	if (lods == 1)
		return 0;

	const auto center = glm::vec4((aabb.max_ + aabb.min_) / 2.0f, 1.0f);
	const auto radius = glm::length(aabb.max_ - aabb.min_) / 2;

	const auto p = (lod_system::near_plane * radius) / glm::dot(lod_system::view_dir, (lod_system::view_pos - center));
	const auto ratio = (glm::pi<float>() * p * p); // ratio of pixel/screen
	
	lods--;
	while (lods != 0 && ratio < (1.0f / pow(2, lods)))
	{
		lods--;
	}
	return lods;

	/*
	if (lods > 7 && ratio < 1.0f / 128)
	{
		return 7;
	}
	if (lods > 6 && ratio < 1.0f / 64)
	{
		return 6;
	}
	if (lods > 5 && ratio < 1.0f / 32)
	{
		return 5;
	}
	if (lods > 4 && ratio < 1.0f / 16)
	{
		return 4;
	}
	if (lods > 3 && ratio < 1.0f / 8)
	{
		return 3;
	}
	if (lods > 2 && ratio < 1.0f / 4)
	{
		return 2;
	}
	if (lods > 1 && ratio < 1.0f / 2)
	{
		return 1;
	}
	return 0;*/
	
}