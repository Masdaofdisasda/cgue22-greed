#include "UBO.h"

UBO::UBO()
{
	glGenBuffers(1, &ubo_id_);
}


// lights
void UBO::fill_buffer(std::vector<directional_light>& buffer_data) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_id_);
	glBufferData(GL_UNIFORM_BUFFER, buffer_data.size() * sizeof(directional_light), buffer_data.data(), GL_STATIC_READ);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::fill_buffer(std::vector<positional_light>& buffer_data) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_id_);
	glBufferData(GL_UNIFORM_BUFFER, buffer_data.size() * sizeof(positional_light), buffer_data.data(), GL_STATIC_READ);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Per Frame Data
void UBO::fill_buffer(PerFrameData pfbuffer)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_id_);
	glNamedBufferStorage(ubo_id_, sizeof(PerFrameData), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_id_, 0, sizeof(PerFrameData));
	update(pfbuffer);
}

void UBO::update(PerFrameData pfbuffer) const
{
	glNamedBufferSubData(ubo_id_, 0, sizeof(PerFrameData), &pfbuffer);
}
