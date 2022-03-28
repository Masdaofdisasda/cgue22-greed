#pragma once

#include "Utils.h"
#include "Texture.h"
#include <vector>

/* class for materials
* every material holds a vector of 5 textures and can return handles to them
*/
class Material
{
public:
	Material(const char* texPath);
	~Material() { Release(); };

	Texture* getAlbedo()
	{
		return &textures[0];
	}
	Texture* getNormalmap()
	{
		return &textures[1];
	}
	Texture* getMetallic()
	{
		return &textures[2];
	}
	Texture* getRoughness()
	{
		return &textures[3];
	}
	Texture* getAOmap()
	{
		return &textures[4];
	}

private:
	
	std::vector<Texture> textures;

	//helper functions for easier loading
	const char* append(const char* texPath, char* texType);

	void Release()
	{
	}

};

