#include "Shader.h"

std::string Shader::read_code_from(const char* file)
{
	std::ifstream ifs(file);
	if (!ifs.is_open())
	{
		std::cout << "ERROR: Couldnt read file" << std::endl;
	}
	std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	return content;
}

std::string Shader::insert_lightcount(std::string code, int dir, int pos)
{
	const std::string d_replace = "dMAXLIGHTS";
	const std::string p_replace = "pMAXLIGHTS";

	std::string d_lights = std::to_string(dir);
	std::string p_lights = std::to_string(pos);

	// replace first (and only) occurence of xReplace and replace it with xLights
	code.replace(code.find(d_replace), d_replace.length(), d_lights);
	code.replace(code.find(p_replace), p_replace.length(), p_lights);

	return code;
}

GLenum Shader::gl_shader_type_from_file_name(const char* file_name) const
{
	if (ends_with(file_name, ".vert"))
		return GL_VERTEX_SHADER;
	if (ends_with(file_name, ".frag"))
		return GL_FRAGMENT_SHADER;
	if (ends_with(file_name, ".geom"))
		return GL_GEOMETRY_SHADER;
	if (ends_with(file_name, ".tesc"))
		return GL_TESS_CONTROL_SHADER;
	if (ends_with(file_name, ".tese"))
		return GL_TESS_EVALUATION_SHADER;
	if (ends_with(file_name, ".comp"))
		return GL_COMPUTE_SHADER;
	assert(false);
	return 0;
}

int Shader::ends_with(const char* s, const char* part) const
{
	return (strstr(s, part) - s) == (strlen(s) - strlen(part));
}

Shader::Shader(const char* file_name)
{
	type = gl_shader_type_from_file_name(file_name);
	shader_id = glCreateShader(type);

	const std::string shader = read_code_from(file_name);
	const char* shader_code = shader.c_str();

	glShaderSource(shader_id, 1, &shader_code, nullptr);
	glCompileShader(shader_id);

	compile_errors();
}

Shader::Shader(const char* file_name, const glm::ivec3 lights)
{
	type = gl_shader_type_from_file_name(file_name);
	shader_id = glCreateShader(type);

	std::string shader = read_code_from(file_name);
	shader = insert_lightcount(shader, lights.x, lights.y);
	const char* shader_code = shader.c_str();


	glShaderSource(shader_id, 1, &shader_code, nullptr);
	glCompileShader(shader_id);

	compile_errors();
}

void Shader::compile_errors() const
{
	GLint succeed;

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &succeed);
	if (succeed == GL_FALSE)
	{
		GLint logSize;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logSize);
		const auto message = new char[logSize];
		glGetShaderInfoLog(shader_id, logSize, nullptr, message);
		std::cerr << message;
		delete[] message;
	}
}
