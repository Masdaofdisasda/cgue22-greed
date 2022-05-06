#pragma once

#include "Shader.h"
#include "buffer.h"
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

	
	void release()
	{
		glDeleteProgram(program_id_);
		program_id_ = 0;
	}

public:

	/// @brief build a shader program from shaders and check for compile errors
	/// all buildFrom() functions do the same thing but with more shaders
	/// @param a is a valid shader
	void build_from(Shader& a) const;
	void build_from(Shader& a, Shader& b) const;
	void build_from(Shader& a, Shader& b, Shader& c) const;
	void build_from(Shader& a, Shader& b, Shader& c, Shader& d) const;
	void build_from(Shader& a, Shader& b, Shader& c, Shader& d, Shader& e) const;

	/// @brief creates an OpenGL handle, program needs to call some of the buildfrom()
	/// functions before Use(), otherwise the app may crash, this constructor makes
	/// member programs possible
	program();

	~program() { release(); }

	/// @brief makes this the currently active shader program, called before glDraw()
	void use() const;

	// ensure RAII compliance
	program(const program&) = delete;
	program& operator=(const program&) = delete;

	program(program&& other)noexcept : program_id_(other.program_id_)
	{
		other.program_id_ = 0; //Use the "null" ID for the old object.
	}

	program& operator=(program&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			release();
			//obj_ is now 0.
			std::swap(program_id_, other.program_id_);
		}
	}

	// various unifrom set methods
	void bind_light_buffers(buffer* directional, buffer* positional);
	void setu_int(const std::string& name, int value) const;
	void set_int(const std::string& name, int value) const;
	void set_float(const std::string& name, float value) const;
	void set_vec3(const std::string& name, glm::vec3 value) const;
	void set_vec4(const std::string& name, glm::vec4 value) const;
	void set_mat4(const std::string& name, glm::mat4 value) const;

	GLuint get_handle() const { return program_id_; }

	void compile_errors() const;
};