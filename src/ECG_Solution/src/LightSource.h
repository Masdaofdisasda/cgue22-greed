#pragma once
#include "Utils.h"
#include "vector"

// setup light sources
struct DirectionalLight
{
	glm::vec4 direction;

	glm::vec4 intensity;
};

struct PositionalLight
{
	glm::vec4 position;
	glm::vec4 attenuation;

	glm::vec4 intensity;
};

struct SpotLight
{
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 Angles;
	glm::vec4 attenuation;

	glm::vec4 intensity;
};

struct LightSources
{
	std::vector <DirectionalLight> directional;
	std::vector <PositionalLight> point;
	std::vector <SpotLight> spot;
};