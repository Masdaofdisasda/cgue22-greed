#pragma once

#include "Utils.h"

class Texture
{
private:
	GLuint tex_ID = 0;
	string path;
	GLenum type_ = 0;

	void Release()
	{
		glDeleteTextures(1, &tex_ID);
		tex_ID = 0;
	}

	int getNumMipMapLevels2D(int w, int h);

public:
	Texture();

	Texture(GLenum type, int width, int height, GLenum internalFormat);
	~Texture() { Release(); }

	void load(const char* texPath, int texUnit = 0);
	bool equals(string tex);

	// ensure RAII compliance
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& other) noexcept : tex_ID(other.tex_ID)
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

	GLuint* getID() {return &tex_ID;}
	GLuint getHandle() const { return tex_ID; }
	string getPath() { return path; }
};
