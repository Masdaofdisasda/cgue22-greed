#pragma once

#include "Shader.h"
#include "UBO.h"
#include "Level.h"

class level;
/// @brief Program is a shader program compiled from 1 to 5 shaders
/// contains a handle, builds and error checks the programm
/// can set unfiform variables for a shader
class program
{
private:
	// Reference ID of the Shader Program
	GLuint program_id_ = 0;

	// Location of light source buffer blocks
	GLuint dirLoc=0, posLoc=0;

	
	void release()
	{
		glDeleteProgram(program_id_);
		program_id_ = 0;
		dirLoc = 0, posLoc = 0;
	}

	void get_uniform_locations();

public:

	/// @brief build a shader program from shaders and check for compile errors
	/// all buildFrom() functions do the same thing but with more shaders
	/// @param a is a valid shader
	void build_from(Shader& a);
	void build_from(Shader& a, Shader& b);
	void build_from(Shader& a, Shader& b, Shader& c);
	void build_from(Shader& a, Shader& b, Shader& c, Shader& d);
	void build_from(Shader& a, Shader& b, Shader& c, Shader& d, Shader& e);

	/// @brief creates an OpenGL handle, program needs to call some of the buildfrom()
	/// functions before Use(), otherwise the app may crash, this constructor makes
	/// member programs possible
	program();

	~program() { release(); }

	/// @brief makes this the currently active shader program, called before glDraw()
	void use() const;

	/// @brief binds IBL textures to uniforms 5,6,7 and 8 (same order as the parameters)
	/// @param irradiance is the texture handle for the irradiance map
	/// @param pre_filter is the texture handle for the prefilter map
	/// @param bdrf_lut is the texture handle for the bdrflut map
	/// @param enviroment  is the texture handle for the enviroment map
	static void upload_ibl (GLuint irradiance, GLuint pre_filter, GLuint bdrf_lut, GLuint enviroment);

	// ensure RAII compliance
	program(const program&) = delete;
	program& operator=(const program&) = delete;

	program(program&& other)noexcept : program_id_(other.program_id_)
	{
		other.program_id_ = 0; //Use the "null" ID for the old object.
		other.dirLoc = 0;
		other.posLoc = 0;
	}

	program& operator=(program&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			release();
			//obj_ is now 0.
			std::swap(program_id_, other.program_id_);
			std::swap(dirLoc, other.dirLoc);
			std::swap(posLoc, other.posLoc);
		}
	}

	// various unifrom set methods
	void bind_light_buffers(UBO* directional, UBO* positional);
	void setu_int(const std::string& name, int value) const;
	void set_int(const std::string& name, int value) const;
	void set_float(const std::string& name, float value) const;
	void set_vec3(const std::string& name, glm::vec3 value) const;
	void set_vec4(const std::string& name, glm::vec4 value) const;
	void set_mat4(const std::string& name, glm::mat4 value) const;

	GLuint get_handle() const { return program_id_; }

	void compile_errors() const;
};