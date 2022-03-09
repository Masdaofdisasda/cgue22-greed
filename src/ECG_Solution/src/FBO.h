#pragma once
#include "Utils.h"

class FBO
{
public:
	FBO()
	{
		glGenFramebuffers(1, &fbo_ID);
	}
	~FBO() { Release(); }

	// ensure RAII compliance
	FBO(const FBO&) = delete;
	FBO& operator=(const FBO&) = delete;

	FBO(FBO&& other) noexcept : fbo_ID(other.fbo_ID)
	{
		other.fbo_ID = 0; //Use the "null" ID for the old object.
	}

	FBO& operator=(FBO&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(fbo_ID, other.fbo_ID);
		}
	}

private:
	GLuint fbo_ID = 0;

	void Release()
	{
		glDeleteBuffers(1, &fbo_ID);
		fbo_ID = 0;
	}
};
