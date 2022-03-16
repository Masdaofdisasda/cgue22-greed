#include"VAO.h"

VAO::VAO()
{
	// generate VAO
	glGenVertexArrays(1, &vao_ID);
}

void VAO::LinkAttrib(VBO* VBO, GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset)
{
	// bind position or color to VAO
	(*VBO).Bind();
	glVertexAttribPointer(layout, numComp, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	(*VBO).Unbind();
}

// bind VAO to current context
void VAO::Bind()
{
	glBindVertexArray(vao_ID);

}

// binds current context to nothing, releases VAO
void VAO::Unbind()
{
	glBindVertexArray(0);

}
