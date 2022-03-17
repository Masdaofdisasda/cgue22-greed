#include "Program.h"

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

Program::Program()
{
	program_ID = glCreateProgram();
}


void Program::Use()
{
	// activate shader programm 
	glUseProgram(program_ID);
}

void Program::setTextures()
{
	setInt("material.albedo", 0);
	setInt("material.normal", 1);
	setInt("material.metallic", 2);
	setInt("material.roughness", 3);
	setInt("material.ao", 4);
	setInt("material.irradiance", 5);
	setInt("material.prefilter", 6);
	setInt("material.brdfLut", 7);
}

void Program::setSkyboxTextures()
{
	setInt("environment", 0);
}

void Program::Draw(Mesh& mesh)
{

	// load model matrix on shader
	setMat4("model", mesh.model);

	// use texture 
	if (!mesh.getMaterial()->getAlbedo()->equals(albedo))
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.getMaterial()->getAlbedo()->getHandle());
		albedo = mesh.getMaterial()->getAlbedo()->getPath();
	}
	if (!mesh.getMaterial()->getNormalmap()->equals(normal))
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh.getMaterial()->getNormalmap()->getHandle());
		normal = mesh.getMaterial()->getNormalmap()->getPath();
	}
	if (!mesh.getMaterial()->getMetallic()->equals(metallic))
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mesh.getMaterial()->getMetallic()->getHandle());
		metallic = mesh.getMaterial()->getMetallic()->getPath();
	}
	if (!mesh.getMaterial()->getRoughness()->equals(roughness))
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mesh.getMaterial()->getRoughness()->getHandle());
		roughness = mesh.getMaterial()->getRoughness()->getPath();
	}
	if (!mesh.getMaterial()->getAOmap()->equals(ao))
	{
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mesh.getMaterial()->getAOmap()->getHandle());
		ao = mesh.getMaterial()->getAOmap()->getPath();
	}
	if (!mesh.getMaterial()->getCubemap()->equals(cube))
	{
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mesh.getMaterial()->getCubemap()->getIrradianceID());
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mesh.getMaterial()->getCubemap()->getPreFilterID());
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, mesh.getMaterial()->getCubemap()->getBdrfLutID());
		cube = mesh.getMaterial()->getCubemap()->getPath();
	}
	
	

	// draw meshgl
	mesh.BindVAO();
	glDrawElements(GL_TRIANGLES, mesh.getIndicesSize(), GL_UNSIGNED_INT, 0);
}
void Program::DrawSkybox(Mesh& mesh)
{
	setMat4("model", glm::mat4(0));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mesh.getMaterial()->getCubemap()->getEnvironment());

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
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, *directional->getID());
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, *positional->getID());
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, *spot->getID());
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