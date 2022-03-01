#pragma once
#include"Utils.h"
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;
};

class VBO
{
	public:

		// Reference ID of the VBO
		GLuint vbo;

		// creates an Vertex Buffer Object from vertices
		VBO(std::vector<Vertex>& vertices);

		void Bind();
		void Unbind();
		void Delete();
};