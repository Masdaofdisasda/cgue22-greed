#include "Shader.h"


// read shader source code from file and return it as string
std::string read_code_from(const char* file)
{
	std::ifstream ifs(file);
	if (!ifs.is_open())
	{
		std::cout << "ERROR: Couldnt read file" << std::endl;
	}
	std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	return content;
}

// insert light stuct size if needed
std::string Shader::insertLightcount(std::string code)
{
	std::string dReplace = "dMAXLIGHTS";
	std::string pReplace = "pMAXLIGHTS";

	// replace first (and only) occurence of xReplace and replace it with xLights
	code.replace(code.find(dReplace), dReplace.length(), dLights);
	code.replace(code.find(pReplace), pReplace.length(), pLights);

	return code;
}

GLenum Shader::GLShaderTypeFromFileName(const char* fileName)
{
	if (endsWith(fileName, ".vert"))
		return GL_VERTEX_SHADER;
	if (endsWith(fileName, ".frag"))
		return GL_FRAGMENT_SHADER;
	if (endsWith(fileName, ".geom"))
		return GL_GEOMETRY_SHADER;
	if (endsWith(fileName, ".tesc"))
		return GL_TESS_CONTROL_SHADER;
	if (endsWith(fileName, ".tese"))
		return GL_TESS_EVALUATION_SHADER;
	if (endsWith(fileName, ".comp"))
		return GL_COMPUTE_SHADER;
	assert(false);
	return 0;
}

int Shader::endsWith(const char* s, const char* part) {
	return (strstr(s, part) - s) == (strlen(s) - strlen(part));
}

Shader::Shader(const char* fileName)
{
	hasLights = false;

	type = GLShaderTypeFromFileName(fileName);
	shader_ID = glCreateShader(type);

	std::string shader = read_code_from(fileName);
	const char* shaderCode = shader.c_str();


	glShaderSource(shader_ID, 1, &shaderCode, nullptr);
	glCompileShader(shader_ID);

	compileErrors();
}

Shader::Shader(const char* fileName, glm::ivec3 lights)
{
	hasLights = true;
	setLightCounts(lights.x, lights.y);

	type = GLShaderTypeFromFileName(fileName);
	shader_ID = glCreateShader(type);

	std::string shader = read_code_from(fileName);
	shader = insertLightcount(shader);
	const char* shaderCode = shader.c_str();


	glShaderSource(shader_ID, 1, &shaderCode, nullptr);
	glCompileShader(shader_ID);

	compileErrors();
}


void Shader::setLightCounts(int dir, int pos)
{
	// sets light counts
	dLights = std::to_string(dir);
	pLights = std::to_string(pos);
}


// check for compile errors
int Shader::compileErrors()
{
	GLint succeded;

	glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		// get error
		GLint logSize;
		glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(shader_ID, logSize, nullptr, message);
		EXIT_WITH_ERROR(message); // print error
		delete[] message;
	}
}