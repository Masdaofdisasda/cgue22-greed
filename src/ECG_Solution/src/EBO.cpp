#include"EBO.h"

EBO::EBO(std::vector<GLuint>& indices)
{
	std::cout << "create Element Buffer Object (EBO)..." << std::endl;

	// generate EBO, bind buffer, link to indices
	glGenBuffers(1, &ebo_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

// bind EBO to current context
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_ID);
}

// binds current context to nothing, releases EBO
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// deletes EBO
void EBO::Delete()
{
	std::cout << "delete Element Buffer Object (EBO)..." << std::endl;
	glDeleteBuffers(1, &ebo_ID);
}