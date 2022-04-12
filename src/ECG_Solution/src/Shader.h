#pragma once
#include <fstream>
#include "UBO.h"

// read shader source code into char
std::string read_code_from(const char* file);

/// @brief Shader is some GLSL shader from some file location
/// it contains a handle to the shader object and loads, compiles and checks for compilation error
class Shader
{
public:

	// Reference ID of the Shader Program
	GLuint shader_ID = 0;
	GLenum type;
	boolean hasLights;

	const char* fileName;

	// light counts
	std::string dLights, pLights;

	explicit Shader(const char* fileName);
	Shader(const char* fileName, glm::ivec3 lights);

	GLuint* getID() { return &shader_ID; }

	void getUniformLocations();

	void bindBufferBaseToBindingPoint(const std::string& name, UBO value);
	void setuInt(const std::string& name, int value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setMat4(const std::string& name, glm::mat4 value);

	// ensure RAII compliance
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& other)noexcept : shader_ID(other.shader_ID)
	{
		other.shader_ID = 0; //Use the "null" ID for the old object.
	}

	Shader& operator=(Shader&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(shader_ID, other.shader_ID);
		}
	}

	~Shader() { Release(); }

private:
	Shader(GLenum type, const char* text);
	GLenum GLShaderTypeFromFileName(const char* fileName); 
	int endsWith(const char* s, const char* part);
	// set light counts
	void setLightCounts(int dir, int pos);
	// Checks if the different Shaders have compiled properly
	int compileErrors();
	// Replace MAXLIGHTS with correct light count
	std::string insertLightcount(std::string code);

	void Release()
	{
		glDeleteShader(shader_ID);
		shader_ID = 0;
	}
};