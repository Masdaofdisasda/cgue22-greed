#pragma once

#include "Utils.h"
#include "Texture.h"
#include "Cubemap.h"

// descripes Material
class Material
{
public:
	Material(Texture* diff, Texture* spec, Cubemap* cube, glm::vec4 coeffs, float reflection);
	~Material() { Release(); };

	//Texture* diffuse; //todo: add default textures
	//Texture* specular;
	//Cubemap* cubemap;
	glm::vec4 coefficients;
	float reflection;

	Texture* getDiffuse()
	{
		return diffuse;
	}
	Texture* getSpecular()
	{
		return specular;
	}
	Cubemap* getCubemap()
	{
		return cubemap;
	}

	// ensure RAII compliance
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete; 

	Material(Material&& other) noexcept : diffuse(other.diffuse)
	{
		other.diffuse = nullptr; //Use the "null" ID for the old object.
	}

	Material& operator=(Material&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(diffuse, other.diffuse);
			std::swap(specular, other.specular);
			std::swap(cubemap, other.cubemap);
			std::swap(coefficients, other.coefficients);
			std::swap(reflection, other.reflection);
		}
	}

private:
	
	Texture* diffuse; //todo: add default textures
	Texture* specular;
	Cubemap* cubemap;

	void Release()
	{
		diffuse = nullptr;
		specular = nullptr;
		cubemap = nullptr;
	}

};

