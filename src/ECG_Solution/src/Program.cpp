#include "Program.h"

void program::build_from(Shader& a)
{
	glAttachShader(program_id_, *a.get_id());
	glLinkProgram(program_id_);

	compile_errors();
	get_uniform_locations();
}
void program::build_from(Shader& a, Shader& b)
{
	glAttachShader(program_id_, *a.get_id());
	glAttachShader(program_id_, *b.get_id());
	glLinkProgram(program_id_);

	compile_errors();
	get_uniform_locations();
}
void program::build_from(Shader& a, Shader& b, Shader& c)
{
	glAttachShader(program_id_, *a.get_id());
	glAttachShader(program_id_, *b.get_id());
	glAttachShader(program_id_, *c.get_id());
	glLinkProgram(program_id_);

	compile_errors();
	get_uniform_locations();
}
void program::build_from(Shader& a, Shader& b, Shader& c, Shader& d)
{
	glAttachShader(program_id_, *a.get_id());
	glAttachShader(program_id_, *b.get_id());
	glAttachShader(program_id_, *c.get_id());
	glAttachShader(program_id_, *d.get_id());
	glLinkProgram(program_id_);

	compile_errors();
	get_uniform_locations();
}
void program::build_from(Shader& a, Shader& b, Shader& c, Shader& d, Shader& e)
{
	glAttachShader(program_id_, *a.get_id());
	glAttachShader(program_id_, *b.get_id());
	glAttachShader(program_id_, *c.get_id());
	glAttachShader(program_id_, *d.get_id());
	glAttachShader(program_id_, *e.get_id());
	glLinkProgram(program_id_);

	compile_errors();
	get_uniform_locations();

}

program::program()
{
	program_id_ = glCreateProgram();
}

void program::use() const
{
	glUseProgram(program_id_);
}

void program::upload_ibl(const GLuint irradiance, const GLuint pre_filter, const GLuint bdrf_lut, const GLuint enviroment)
{
	const GLuint textures[] = {
				irradiance,
				pre_filter,
				bdrf_lut,
				enviroment };
	glBindTextures(5, 4, textures);
}

// TODO
void program::get_uniform_locations()
{

	dirLoc = glGetUniformBlockIndex(program_id_, "dLightUBlock");
	posLoc = glGetUniformBlockIndex(program_id_, "pLightUBlock");

}

// TODO
void program::bind_light_buffers(UBO* directional, UBO* positional)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, directional->get_id());
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, positional->get_id());
}

void program::setu_int(const std::string& name, int value) const
{
	glUniform1ui(glGetUniformLocation(program_id_, name.c_str()), value);
}

void program::set_int(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(program_id_, name.c_str()), value);
}


void program::set_float(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(program_id_, name.c_str()), value);
}

void program::set_vec3(const std::string& name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]);
}

void program::set_vec4(const std::string& name, glm::vec4 value) const
{
	glUniform4fv(glGetUniformLocation(program_id_, name.c_str()), 1, &value[0]);
}

void program::set_mat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/// @brief check for compile errors
void program::compile_errors() const
{
	GLint succeded;

	glGetProgramiv(program_id_, GL_LINK_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		GLint logSize;
		glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &logSize);
		const auto message = new char[logSize];
		glGetProgramInfoLog(program_id_, logSize, nullptr, message);
		std::cerr <<message;
		delete[] message;
	}
}
