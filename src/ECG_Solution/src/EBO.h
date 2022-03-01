#pragma once
#include"Utils.h"
#include <vector>

class EBO
{
public:

	// Reference ID of the EBO
	GLuint ebo;

	// creates an Element Buffer Object from indices
	EBO(std::vector<GLuint>& indices);

	void Bind();
	void Unbind();
	void Delete();
};