#include"VBO.h"

VBO::VBO(std::vector<Vertex>& vertices)
{
	std::cout << "create Vertex Buffer Object (VBO)..." << std::endl;

	glGenBuffers(1, &vbo_ID);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

// bind VBO to current context
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
}

// binds current context to nothing, releases VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
