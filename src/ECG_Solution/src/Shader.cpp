#include "Shader.h"

/// @brief load shader code from file
/// @param file is the path to some shader file
/// @return a string containing shader code
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

/// @brief replaces placeholder code with actual numbers
/// @param code some shader code in string form with a light source array of size "xMAXLIGHTS"
/// @return the same shader code but with array size set to the number of lights
std::string Shader::insertLightcount(std::string code)
{
	std::string dReplace = "dMAXLIGHTS";
	std::string pReplace = "pMAXLIGHTS";

	// replace first (and only) occurence of xReplace and replace it with xLights
	code.replace(code.find(dReplace), dReplace.length(), dLights);
	code.replace(code.find(pReplace), pReplace.length(), pLights);

	return code;
}


/// @brief find correct shader type based on file ending
/// @param fileName is the path to some shader program
/// @return suitable shader type in gl form
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

/// @brief checks if the end of a filepath matches a file type
/// @param s is the file path eg. "/assets/shader.vert"
/// @param part is the file type eg. ".vert"
/// @return 1 if the condition is true
int Shader::endsWith(const char* s, const char* part) {
	return (strstr(s, part) - s) == (strlen(s) - strlen(part));
}

/// @brief loads and compiles a shader from a file location
/// @param fileName is the location of the shader file
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

/// @brief loads and compiles a shader from a file location
/// @param fileName is the location of the shader file
/// @param lights is the number of lights (dir,point,spot)
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

/// @brief set number of light sources to insert in some shader
/// @param dir number of directional lights
/// @param pos number of positional/point lights
void Shader::setLightCounts(int dir, int pos)
{
	dLights = std::to_string(dir);
	pLights = std::to_string(pos);
}

/// @brief check for shader compilation errors
int Shader::compileErrors()
{
	GLint succeded;

	glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &succeded);
	if (succeded == GL_FALSE)
	{
		GLint logSize;
		glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* message = new char[logSize];
		glGetShaderInfoLog(shader_ID, logSize, nullptr, message);
		EXIT_WITH_ERROR(message);
		delete[] message;
	}
}