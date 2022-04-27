#include "Program.h"

/// @brief build a shader program from shaders and check for compile errors
/// all buildFrom() functions do the same thing but with more shaders
/// @param a is a valid shader
void Program::buildFrom(Shader& a)
{
	glAttachShader(program_ID, *a.getID());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void Program::buildFrom(Shader& a, Shader& b)
{
	glAttachShader(program_ID, *a.getID());
	glAttachShader(program_ID, *b.getID());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void Program::buildFrom(Shader& a, Shader& b, Shader& c)
{
	glAttachShader(program_ID, *a.getID());
	glAttachShader(program_ID, *b.getID());
	glAttachShader(program_ID, *c.getID());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void Program::buildFrom(Shader& a, Shader& b, Shader& c, Shader& d)
{
	glAttachShader(program_ID, *a.getID());
	glAttachShader(program_ID, *b.getID());
	glAttachShader(program_ID, *c.getID());
	glAttachShader(program_ID, *d.getID());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();
}
void Program::buildFrom(Shader& a, Shader& b, Shader& c, Shader& d, Shader& e)
{
	glAttachShader(program_ID, *a.getID());
	glAttachShader(program_ID, *b.getID());
	glAttachShader(program_ID, *c.getID());
	glAttachShader(program_ID, *d.getID());
	glAttachShader(program_ID, *e.getID());
	glLinkProgram(program_ID);

	compileErrors();
	getUniformLocations();

}

/// @brief creates an OpenGL handle, program needs to call some of the buildfrom()
/// functions before Use(), otherwise the app may crash, this constructor makes
/// member programs possible
Program::Program()
{
	program_ID = glCreateProgram();
}

/// @brief makes this the currently active shader program, called before glDraw()
void Program::Use()
{
	glUseProgram(program_ID);
}

/// @brief binds IBL textures to uniforms 5,6,7 and 8 (same order as the parameters)
/// @param Irradiance is the texture handle for the irradiance map
/// @param PreFilter is the texture handle for the prefilter map
/// @param BdrfLut is the texture handle for the bdrflut map
/// @param Enviroment  is the texture handle for the enviroment map
void Program::uploadIBL(GLuint Irradiance, GLuint PreFilter, GLuint BdrfLut, GLuint Enviroment) const
{
	const GLuint textures[] = {
				Irradiance,
				PreFilter,
				BdrfLut,
				Enviroment };
	glBindTextures(5, 4, textures);
}

// TODO
void Program::getUniformLocations()
{

	dirLoc = glGetUniformBlockIndex(program_ID, "dLightUBlock");
	posLoc = glGetUniformBlockIndex(program_ID, "pLightUBlock");

}

// TODO
void Program::bindLightBuffers(UBO* directional, UBO* positional)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, directional->getID());
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, positional->getID());
}

void Program::setuInt(const std::string& name, int value)
{
	glUniform1ui(glGetUniformLocation(program_ID, name.c_str()), value);
}

void Program::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(program_ID, name.c_str()), value);
}


void Program::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(program_ID, name.c_str()), value);
}

void Program::setVec3(const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(program_ID, name.c_str()), 1, &value[0]);
}

void Program::setVec4(const std::string& name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(program_ID, name.c_str()), 1, &value[0]);
}

void Program::setMat4(const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(program_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/// @brief check for compile errors
void Program::compileErrors()
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