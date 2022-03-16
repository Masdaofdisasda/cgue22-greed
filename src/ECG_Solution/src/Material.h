#pragma once

#include "Utils.h"
#include "Texture.h"
#include "Cubemap.h"

// descripes Material
class Material
{
public:
	Material(Texture* alb, Texture* norm, Texture* metal, Texture* rough, Texture* ao, Cubemap* cube);
	~Material() { Release(); };

	Texture* getAlbedo()
	{
		return albedo;
	}
	Texture* getNormalmap()
	{
		return normal;
	}
	Texture* getMetallic()
	{
		return metallic;
	}
	Texture* getRoughness()
	{
		return roughness;
	}
	Texture* getAOmap()
	{
		return ambientocclusion;
	}
	Cubemap* getCubemap()
	{
		return cubemap;
	}

	// ensure RAII compliance
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete; 

	Material(Material&& other) noexcept : albedo(other.albedo)
	{
		other.albedo = nullptr; //Use the "null" ID for the old object.
	}

	Material& operator=(Material&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(albedo, other.albedo);
			std::swap(normal, other.normal);
			std::swap(metallic, other.metallic);
			std::swap(roughness, other.roughness);
			std::swap(ambientocclusion, other.ambientocclusion);
			std::swap(cubemap, other.cubemap);
		}
	}

private:
	
	Texture* albedo;
	Texture* normal;
	Texture* metallic;
	Texture* roughness;
	Texture* ambientocclusion;
	Cubemap* cubemap;

	void Release()
	{
		albedo = nullptr;
		normal = nullptr;
		metallic = nullptr;
		roughness = nullptr;
		ambientocclusion = nullptr;
		cubemap = nullptr;
	}

};

