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

std::string Shader::insertLightcount(std::string code)
{
	std::string dReplace = "dMAXLIGHTS";
	std::string pReplace = "pMAXLIGHTS";
	std::string sReplace = "sMAXLIGHTS";

	// replace first (and only) occurence of xReplace and replace it with xLights
	code.replace(code.find(dReplace), dReplace.length(), dLights);
	code.replace(code.find(pReplace), pReplace.length(), pLights);
	code.replace(code.find(sReplace), sReplace.length(), sLights);

	return code;
}

// creates shader programm with vertex and fragment shader
Shader::Shader(const char*  vertexSource, const char* fragmentSource, int dir, int pos, int spot, int shadeType)
{
	setLightCounts(dir, pos, spot);
	name = vertexSource;

	std::cout << "reading shader code from files..." << std::endl;
	std::string vert = read_code_from(vertexSource);
	std::string frag = read_code_from(fragmentSource);
	if (shadeType == 0) // phong
	{
		frag = insertLightcount(frag);
	}
	else if (shadeType == 1) // gouraud
	{
		vert = insertLightcount(vert);
	}

	//std::cout << frag <<std::endl;
	//std::cout << vert << std::endl;

	// read shader code from source and save as char
	const char* vertex_code = vert.c_str();
	const char* fragment_code = frag.c_str();

	// build vertex shader from vertex code
	std::cout << "compiling vertex shader..." << std::endl;
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_code, nullptr);
	glCompileShader(vertex_shader);
	// check for compile error

	compileErrors(vertex_shader, "shader");

	// build fragment shader from fragment code
	std::cout << "compiling fragment shader..." << std::endl;
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_code, nullptr);
	glCompileShader(fragment_shader);
	// check for compile error
	compileErrors(fragment_shader, "shader");

	// build shader porgramm 
	std::cout << "compile shader program from vertex and fragment shader..." << std::endl;
	shader_program = glCreateProgram();
	// attach vertex and fragment shaders
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	// link shaders together
	glLinkProgram(shader_program);
	//check for errors
	compileErrors(shader_program, "program");

	// delete useless shaders (only needed for recompiling)
	std::cout << "delete shader objects..." << std::endl;
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	std::cout << "get uniform locations..." << std::endl;
	getUniformLocations();

	std::cout << std::endl;
}

void Shader::setLightCounts(int dir, int pos, int spot)
{
	// sets light counts
	dLights = std::to_string(dir);
	pLights = std::to_string(pos);
	sLights = std::to_string(spot);
}


void Shader::Use()
{
	// activate shader programm 
	glUseProgram(shader_program);

	//getUniformLocations();
}

void Shader::getUniformLocations()
{

	dirLoc = glGetUniformBlockIndex(shader_program, "dLightUBlock");
	posLoc = glGetUniformBlockIndex(shader_program, "pLightUBlock");
	spotLoc = glGetUniformBlockIndex(shader_program, "sLightUBlock");

}


void Shader::bindBufferBaseToBindingPoint(const std::string &name, UBO value)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(shader_program, name.c_str()), *value.getID());
}

void Shader::setuInt(const std::string& name, int value)
{
	glUniform1ui(glGetUniformLocation(shader_program, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(shader_program, name.c_str()), value);
}


void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(shader_program, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(shader_program, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

Shader::~Shader()
{
	//std::cout << "delete shader programm..." << std::endl;
	// delete shader programm 
	glDeleteProgram(shader_program);
}

// check for compile errors
int Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint succeded;

	std::cout << "check for compile errors..." << std::endl;

	if (type == "shader")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &succeded);
		if (succeded == GL_FALSE)
		{
			// get error
			GLint logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			GLchar* message = new char[logSize];
			glGetShaderInfoLog(shader, logSize, nullptr, message);
			EXIT_WITH_ERROR(message); // print error
			delete[] message;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &succeded);
		if (succeded == GL_FALSE)
		{
			// get error
			GLint logSize;
			glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			GLchar* message = new char[logSize];
			glGetProgramInfoLog(shader, logSize, nullptr, message);
			EXIT_WITH_ERROR(message); // print error
			delete[] message;
		}
	}
}