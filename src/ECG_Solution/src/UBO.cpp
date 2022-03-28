#include "UBO.h"

UBO::UBO()
{
	glGenBuffers(1, &ubo_ID);
}


// lights
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

// Per Frame Data
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

// Per Frame Render Settings
void UBO::fillBuffer(PerFrameSettings pfsetbuffer)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_ID);
	glNamedBufferStorage(ubo_ID, sizeof(PerFrameSettings), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 4, ubo_ID, 0, sizeof(PerFrameSettings));
	Update(pfsetbuffer);
}

void UBO::Update(PerFrameSettings pfsetbuffer)
{
	glNamedBufferSubData(ubo_ID, 0, sizeof(PerFrameSettings), &pfsetbuffer);
}
