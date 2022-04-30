#include "Shader.h"

/// @brief load shader code from file
/// @param file is the path to some shader file
/// @return a string containing shader code
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

/// @brief replaces placeholder code with actual numbers
/// @param code some shader code in string form with a light source array of size "xMAXLIGHTS"
/// @return the same shader code but with array size set to the number of lights
std::string Shader::insert_lightcount(std::string code)
{
	const std::string d_replace = "dMAXLIGHTS";
	const std::string p_replace = "pMAXLIGHTS";

	// replace first (and only) occurence of xReplace and replace it with xLights
	code.replace(code.find(d_replace), d_replace.length(), d_lights);
	code.replace(code.find(p_replace), p_replace.length(), p_lights);

	return code;
}


/// @brief find correct shader type based on file ending
/// @param file_name is the path to some shader program
/// @return suitable shader type in gl form
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

/// @brief checks if the end of a filepath matches a file type
/// @param s is the file path eg. "/assets/shader.vert"
/// @param part is the file type eg. ".vert"
/// @return 1 if the condition is true
int Shader::ends_with(const char* s, const char* part) const
{
	return (strstr(s, part) - s) == (strlen(s) - strlen(part));
}

/// @brief loads and compiles a shader from a file location
/// @param file_name is the location of the shader file
Shader::Shader(const char* file_name)
{
	has_lights = false;

	type = gl_shader_type_from_file_name(file_name);
	shader_id = glCreateShader(type);

	const std::string shader = read_code_from(file_name);
	const char* shader_code = shader.c_str();

	glShaderSource(shader_id, 1, &shader_code, nullptr);
	glCompileShader(shader_id);

	compile_errors();
}

/// @brief loads and compiles a shader from a file location
/// @param file_name is the location of the shader file
/// @param lights is the number of lights (dir,point,spot)
Shader::Shader(const char* file_name, const glm::ivec3 lights)
{
	has_lights = true;
	set_light_counts(lights.x, lights.y);

	type = gl_shader_type_from_file_name(file_name);
	shader_id = glCreateShader(type);

	std::string shader = read_code_from(file_name);
	shader = insert_lightcount(shader);
	const char* shader_code = shader.c_str();


	glShaderSource(shader_id, 1, &shader_code, nullptr);
	glCompileShader(shader_id);

	compile_errors();
}

/// @brief set number of light sources to insert in some shader
/// @param dir number of directional lights
/// @param pos number of positional/point lights
void Shader::set_light_counts(const int dir, const int pos)
{
	d_lights = std::to_string(dir);
	p_lights = std::to_string(pos);
}

/// @brief check for shader compilation errors
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
