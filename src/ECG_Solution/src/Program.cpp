#include "Program.h"

Program::Program(Shader& a, Shader& b)
{
	program_ID = glCreateProgram();
	glAttachShader(program_ID, *a.getID());
	glAttachShader(program_ID, *b.getID());
	glLinkProgram(program_ID);

	compileErrors();

	getUniformLocations();
}


void Program::Use()
{
	// activate shader programm 
	glUseProgram(program_ID);
}

void Program::uploadMaterial(Material* material)
{
	setInt("material.albedo", 0);
	setInt("material.specular", 1);
	setInt("material.irradiance", 2);
	setVec4("material.coefficients", material->coefficients);
	setFloat("material.reflection", material->reflection);
}

void Program::Draw(Mesh& mesh)
{

	// load model matrix on shader
	setMat4("model", mesh.model);

	// use texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *mesh.getMaterial()->getDiffuse()->getID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *mesh.getMaterial()->getSpecular()->getID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *mesh.getMaterial()->getCubemap()->getID());

	// load mesh on shader
	uploadMaterial(mesh.getMaterial());

	// draw meshgl
	mesh.BindVAO();
	glDrawElements(GL_TRIANGLES, mesh.getIndicesSize(), GL_UNSIGNED_INT, 0);
}

void Program::getUniformLocations()
{

	dirLoc = glGetUniformBlockIndex(program_ID, "dLightUBlock");
	posLoc = glGetUniformBlockIndex(program_ID, "pLightUBlock");
	spotLoc = glGetUniformBlockIndex(program_ID, "sLightUBlock");

}


void Program::bindLightBuffers(UBO* directional, UBO* positional, UBO* spot)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, *directional->getID());
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, *positional->getID());
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, *spot->getID());
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

// check for compile errors
int Program::compileErrors()
{
	GLint succeded;

	glGetProgramiv(program_ID, GL_LINK_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{

		// get error
		GLint logSize;
		glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetProgramInfoLog(program_ID, logSize, nullptr, message);
		EXIT_WITH_ERROR(message); // print error
		delete[] message;
	}
}