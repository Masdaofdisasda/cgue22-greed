#include "UBO.h"

UBO::UBO()
{
	glGenBuffers(1, &ubo_ID);
}

void UBO::fillBuffer(std::vector<DirectionalLight>& bufferData)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
	glBufferData(GL_UNIFORM_BUFFER, bufferData.size() * sizeof(DirectionalLight), bufferData.data(), GL_STATIC_READ);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::fillBuffer(std::vector<PositionalLight>& bufferData)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
	glBufferData(GL_UNIFORM_BUFFER, bufferData.size() * sizeof(PositionalLight), bufferData.data(), GL_STATIC_READ);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::fillBuffer(std::vector<SpotLight>& bufferData)
{

	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
	glBufferData(GL_UNIFORM_BUFFER, bufferData.size() * sizeof(SpotLight), bufferData.data(), GL_STATIC_READ);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::fillBuffer(PerFrameData pfbuffer)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
	glNamedBufferStorage(ubo_ID, sizeof(PerFrameData), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_ID, 0, sizeof(PerFrameData));
	Update(pfbuffer);
}

void UBO::Update(PerFrameData pfbuffer)
{
	glNamedBufferSubData(ubo_ID, 0, sizeof(PerFrameData), &pfbuffer);
}
