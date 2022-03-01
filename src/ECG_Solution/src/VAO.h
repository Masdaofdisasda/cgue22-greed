#pragma once
#include"Utils.h"
#include"VBO.h"

class VAO
{
public:

	// Reference ID of the VAO
	GLuint vao;

	// creates Vertex Arrays Object
	VAO();

	// links an VBO color and position to an VAO
	void LinkAttrib(VBO VBO, GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();
};