#pragma once
#include "Utils.h"
#include "vector"

// setup light sources
struct directional_light
{
	glm::vec4 direction;

	glm::vec4 intensity;
};

struct positional_light
{
	glm::vec4 position;

	glm::vec4 intensity;
};

struct light_sources
{
	std::vector <directional_light> directional;
	std::vector <positional_light> point;
};
