#pragma once

#include "Utils.h"
#include "Texture.h"
#include "Cubemap.h"

// descripes Material
class Material
{
public:
	Material(const char* texPath, const char* cubePath);
	Material(const char* cubePath);
	~Material() { Release(); };

	Texture* getAlbedo()
	{
		return &albedo;
	}
	Texture* getNormalmap()
	{
		return &normal;
	}
	Texture* getMetallic()
	{
		return &metallic;
	}
	Texture* getRoughness()
	{
		return &roughness;
	}
	Texture* getAOmap()
	{
		return &ambientocclusion;
	}
	Cubemap* getCubemap()
	{
		return &cubemap;
	}

	// ensure RAII compliance
	
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete; 

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
	
	Texture albedo;
	Texture normal;
	Texture metallic;
	Texture roughness;
	Texture ambientocclusion;
	Cubemap cubemap;

	const char* append(const char* texPath, char* texType);

	void Release()
	{
		free(&albedo);
		free(&normal);
		free(&metallic);
		free(&roughness);
		free(&ambientocclusion);
		free(&cubemap);
	}

};

