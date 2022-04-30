#include "buffer.h"

buffer::buffer(const GLenum type) : type_(type)
{
	glCreateBuffers(1, &buffer_id_);
}

void buffer::reserve_memory(const GLuint binding, const GLsizeiptr size, const void* data) const
{
	if (type_)
		glBindBuffer(type_, buffer_id_);
	glNamedBufferStorage(buffer_id_, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(type_, binding, buffer_id_, 0, size);

	update(size, data);
}

void buffer::reserve_memory(const GLsizeiptr size, const void* data) const
{
	if (type_)
		glBindBuffer(type_, buffer_id_);
	glNamedBufferStorage(buffer_id_, size, nullptr, GL_DYNAMIC_STORAGE_BIT);

	update(size, data);
}

void buffer::update(const GLsizeiptr size, const void* data) const
{
	glNamedBufferSubData(buffer_id_, 0, size, data);
}
