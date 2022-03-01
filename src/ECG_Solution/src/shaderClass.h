#pragma once
#include "Utils.h"
#include <fstream>
#include "UBO.h"

// read shader source code into char
std::string read_code_from(const char* file);

class Shader
{
public:

	// Reference ID of the Shader Program
	GLuint shader_program;

	string name;

	// location of the model matrix
	GLuint modelLoc;

	// location of the view projection matrix
	GLuint viewProLoc;


	// Location of light source buffer blocks
	GLuint dirLoc, posLoc, spotLoc;

	// Location of light source count
	GLuint dCountLoc, pCountLoc, sCountLoc;
	
	// Location of material struct
	GLuint mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;

	// Location of view position
	GLuint viewPosLoc;

	// light counts
	std::string dLights, pLights, sLights;

	// Constructor, builds shader programm from vertex- and fragement shader
	//					code location			code location	amount of dir/pos/spot lights	0 = lights in frag, 1 = lights in vert
	Shader(const char* vertexSource, const char* fragmentSource, int dir, int pos, int spot, int shadeType);

	// Activates the Shader Program
	void Use();

	void getUniformLocations();

	void bindBufferBaseToBindingPoint(const std::string& name, UBO value);
	void setuInt(const std::string& name, int value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec3(const std::string& name, glm::vec3 value);
	void setVec4(const std::string& name, glm::vec4 value);
	void setMat4(const std::string& name, glm::mat4 value);

	// Deletes the Shader Program
	~Shader();

private:

	// set light counts
	void setLightCounts(int dir, int pos, int spot);
	// Checks if the different Shaders have compiled properly
	int compileErrors(unsigned int shader, const char* type);
	// Replace MAXLIGHTS with correct light count
	std::string insertLightcount(std::string code);
};