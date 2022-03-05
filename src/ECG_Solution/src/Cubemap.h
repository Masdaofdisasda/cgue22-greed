#pragma once
#include "Utils.h"

class Cubemap
{
private:
	GLuint cube_ID = 0;

	void Release()
	{
		glDeleteTextures(1, &cube_ID);
		cube_ID = 0;
	}

public:
	Cubemap(const char* texPath = "assets/textures/cubemap");
	~Cubemap() { Release(); }

	// ensure RAII compliance
	Cubemap(const Cubemap&) = delete;
	Cubemap& operator=(const Cubemap&) = delete;

	Cubemap(Cubemap&& other) : cube_ID(other.cube_ID)
	{
		other.cube_ID = 0; //Use the "null" ID for the old object.
	}

	Cubemap& operator=(Cubemap&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(cube_ID, other.cube_ID);
		}
	}

	GLuint* getID() { return &cube_ID; }
};
