#pragma once
#include "Utils.h"

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

struct Material
{
	int tex0;
	int tex1;
	int tex2;
	glm::vec4 coefficients;
	float reflection;
};