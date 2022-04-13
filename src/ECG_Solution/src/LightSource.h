#pragma once
#include "Utils.h"
#include "vector"

#ifndef _LIGHT_SOURCES_
#define _LIGHT_SOURCES_
// setup light sources
struct DirectionalLight
{
	glm::vec4 direction;

	glm::vec4 intensity;
};

struct PositionalLight
{
	glm::vec4 position;

	glm::vec4 intensity;
};

struct LightSources
{
	std::vector <DirectionalLight> directional;
	std::vector <PositionalLight> point;
};

#endif