#include "Program.h"

/// @brief build a shader program from shaders and check for compile errors
/// all buildFrom() functions do the same thing but with more shaders
/// @param a is a valid shader
void program::buildFrom(Shader& a)
{
	glAttachShader(program_ID, *a.get_id());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void program::buildFrom(Shader& a, Shader& b)
{
	glAttachShader(program_ID, *a.get_id());
	glAttachShader(program_ID, *b.get_id());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void program::buildFrom(Shader& a, Shader& b, Shader& c)
{
	glAttachShader(program_ID, *a.get_id());
	glAttachShader(program_ID, *b.get_id());
	glAttachShader(program_ID, *c.get_id());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void program::buildFrom(Shader& a, Shader& b, Shader& c, Shader& d)
{
	glAttachShader(program_ID, *a.get_id());
	glAttachShader(program_ID, *b.get_id());
	glAttachShader(program_ID, *c.get_id());
	glAttachShader(program_ID, *d.get_id());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void program::buildFrom(Shader& a, Shader& b, Shader& c, Shader& d, Shader& e)
{
	glAttachShader(program_ID, *a.get_id());
	glAttachShader(program_ID, *b.get_id());
	glAttachShader(program_ID, *c.get_id());
	glAttachShader(program_ID, *d.get_id());
	glAttachShader(program_ID, *e.get_id());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();

}

/// @brief creates an OpenGL handle, program needs to call some of the buildfrom()
/// functions before Use(), otherwise the app may crash, this constructor makes
/// member programs possible
program::program()
{
	program_ID = glCreateProgram();
}

/// @brief makes this the currently active shader program, called before glDraw()
void program::Use()
{
	glUseProgram(program_ID);
}

/// @brief binds IBL textures to uniforms 5,6,7 and 8 (same order as the parameters)
/// @param Irradiance is the texture handle for the irradiance map
/// @param PreFilter is the texture handle for the prefilter map
/// @param BdrfLut is the texture handle for the bdrflut map
/// @param Enviroment  is the texture handle for the enviroment map
void program::uploadIBL(GLuint Irradiance, GLuint PreFilter, GLuint BdrfLut, GLuint Enviroment) const
{
	const GLuint textures[] = {
				Irradiance,
				PreFilter,
				BdrfLut,
				Enviroment };
	glBindTextures(5, 4, textures);
}

// TODO
void program::getUniformLocations()
{

	dirLoc = glGetUniformBlockIndex(program_ID, "dLightUBlock");
	posLoc = glGetUniformBlockIndex(program_ID, "pLightUBlock");

}

// TODO
void program::bindLightBuffers(UBO* directional, UBO* positional)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, directional->get_id());
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, positional->get_id());
}

void program::setuInt(const std::string& name, int value)
{
	glUniform1ui(glGetUniformLocation(program_ID, name.c_str()), value);
}

void program::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(program_ID, name.c_str()), value);
}


void program::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(program_ID, name.c_str()), value);
}

void program::setVec3(const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(program_ID, name.c_str()), 1, &value[0]);
}

void program::setVec4(const std::string& name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(program_ID, name.c_str()), 1, &value[0]);
}

void program::setMat4(const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(program_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/// @brief check for compile errors
void program::compileErrors()
{
	GLint succeded;

	glGetProgramiv(program_ID, GL_LINK_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		GLint logSize;
		glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetProgramInfoLog(program_ID, logSize, nullptr, message);
		std::cerr <<message;
		delete[] message;
	}
}