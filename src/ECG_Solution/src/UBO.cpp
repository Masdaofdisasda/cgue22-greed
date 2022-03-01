#include "UBO.h"


UBO::UBO(std::vector<DirectionalLight>& bufferData)
{
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, bufferData.size() * sizeof(DirectionalLight), bufferData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UBO::UBO(std::vector<PositionalLight>& bufferData)
{
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, bufferData.size() * sizeof(PositionalLight), bufferData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UBO::UBO(std::vector<SpotLight>& bufferData)
{
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, bufferData.size() * sizeof(SpotLight), bufferData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::bindBufferBaseToBindingPoint(const GLuint bindingPoint)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
}

UBO::~UBO()
{
	std::cout << "delete Uniform Buffer Object (UBO)..." << std::endl;
	glDeleteBuffers(1, &ubo);
}