#pragma once
#include <fstream>
#include "Utils.h"


/// @brief Shader is some GLSL shader from some file location
/// it contains a handle to the shader object and loads, compiles and checks for compilation error
class Shader
{
public:

	// Reference ID of the Shader Program
	GLuint shader_id = 0;
	GLenum type{};

	/// @brief loads and compiles a shader from a file location
	/// @param file_name is the location of the shader file
	explicit Shader(const char* file_name);

	/// @brief loads and compiles a shader from a file location
	/// @param file_name is the location of the shader file
	/// @param lights is the number of lights (dir,point,spot)
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
	/// @brief find correct shader type based on file ending
	/// @param file_name is the path to some shader program
	/// @return suitable shader type in gl form
	GLenum gl_shader_type_from_file_name(const char* file_name) const;

	/// @brief checks if the end of a filepath matches a file type
	/// @param s is the file path eg. "/assets/shader.vert"
	/// @param part is the file type eg. ".vert"
	/// @return 1 if the condition is true
	int ends_with(const char* s, const char* part) const;

	/// @brief check for shader compilation errors
	void compile_errors() const;

	/// @brief replaces placeholder code with actual numbers
	/// @param code some shader code in string form with a light source array of size "xMAXLIGHTS"
	/// @return the same shader code but with array size set to the number of lights
	std::string insert_lightcount(std::string code, int dir, int pos);

	/// @brief load shader code from file
	/// @param file is the path to some shader file
	/// @return a string containing shader code
	static std::string read_code_from(const char* file);

	void release()
	{
		glDeleteShader(shader_id);
		shader_id = 0;
	}
};