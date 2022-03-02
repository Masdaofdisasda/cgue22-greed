#pragma once
#include "Utils.h"

class Texture
{
private:
	GLuint tex_ID = 0;

	void Release()
	{
		glDeleteBuffers(1, &tex_ID);
		tex_ID = 0;
	}

public:
	Texture(const char* texPath, int texUnit = 0);
	~Texture() { Release(); }

	// ensure RAII compliance
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& other) : tex_ID(other.tex_ID)
	{
		other.tex_ID = 0; //Use the "null" ID for the old object.
	}

	Texture& operator=(Texture&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(tex_ID, other.tex_ID);
		}
	}

	GLuint* getID() { return &tex_ID; }
};
