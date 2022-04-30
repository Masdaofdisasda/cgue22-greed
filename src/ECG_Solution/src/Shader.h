#pragma once
#include <fstream>
#include "UBO.h"


/// @brief Shader is some GLSL shader from some file location
/// it contains a handle to the shader object and loads, compiles and checks for compilation error
class Shader
{
public:

	// Reference ID of the Shader Program
	GLuint shader_id = 0;
	GLenum type{};
	boolean has_lights{};

	// light counts
	std::string d_lights, p_lights;

	explicit Shader(const char* file_name);
	Shader(const char* file_name, glm::ivec3 lights);

	GLuint* get_id() { return &shader_id; }

	// ensure RAII compliance
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& other)noexcept : shader_id(other.shader_id)
	{
		other.shader_id = 0; //Use the "null" ID for the old object.
	}

	Shader& operator=(Shader&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			release();
			//obj_ is now 0.
			std::swap(shader_id, other.shader_id);
		}
	}

	~Shader() { release(); }

private:
	GLenum gl_shader_type_from_file_name(const char* file_name) const; 
	int ends_with(const char* s, const char* part) const;
	// set light counts
	void set_light_counts(int dir, int pos);
	// Checks if the different Shaders have compiled properly
	void compile_errors() const;
	// Replace MAXLIGHTS with correct light count
	std::string insert_lightcount(std::string code);

	// read shader source code into char
	static std::string read_code_from(const char* file);

	void release()
	{
		glDeleteShader(shader_id);
		shader_id = 0;
	}
};