#pragma once

#include "Utils.h"
#include "Shader.h"
#include "Mesh.h"

class Program
{
private:
	// Reference ID of the Shader Program
	GLuint program_ID = 0;

	// Location of light source buffer blocks
	GLuint dirLoc=0, posLoc=0, spotLoc=0;
	
	void Release()
	{
		glDeleteProgram(program_ID);
		program_ID = 0;
		dirLoc = 0, posLoc = 0, spotLoc = 0;
	}

	void getUniformLocations();

public:
	Program(Shader& a, Shader& b);

	~Program() { Release(); }

	void Use();

	void uploadMaterial(Material* material);

	void Draw(Mesh& mesh); // draws triangles

	// ensure RAII compliance
	Program(const Program&) = delete;
	Program& operator=(const Program&) = delete;

	Program(Program&& other)noexcept : program_ID(other.program_ID)
	{
		other.program_ID = 0; //Use the "null" ID for the old object.
		other.dirLoc = 0;
		other.posLoc = 0;
		other.spotLoc = 0;
	}

	Program& operator=(Program&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(program_ID, other.program_ID);
			std::swap(dirLoc, other.dirLoc);
			std::swap(posLoc, other.posLoc);
			std::swap(spotLoc, other.spotLoc);
		}
	}

	void bindLightBuffers(UBO* directional, UBO* positional, UBO* spot);
	void setuInt(const std::string& name, int value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setMat4(const std::string& name, glm::mat4 value);

	int compileErrors();
};

