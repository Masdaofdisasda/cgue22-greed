#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>

struct Vertex // for pbr shader
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};


// class for Vertex Buffer Objects
class VBO
{
	private:

		// Reference ID of the VBO
		GLuint vbo_ID = 0;

		void Release()
		{
		  glDeleteBuffers(1, &vbo_ID);
		  vbo_ID = 0;
		}

	public:

		// creates an Vertex Buffer Object from vertices
		VBO(std::vector<Vertex>& vertices);

		~VBO() {Release();}

		// ensure RAII compliance
		VBO(const VBO&) = delete;
		VBO &operator=(const VBO&) = delete; 

		VBO(VBO&& other) noexcept : vbo_ID(other.vbo_ID)
		{
			other.vbo_ID = 0; //Use the "null" ID for the old object.
		}

		VBO& operator=(VBO&& other)
		{
			//ALWAYS check for self-assignment.
			if (this != &other)
			{
				Release();
				//obj_ is now 0.
				std::swap(vbo_ID, other.vbo_ID);
			}
		}

		void Bind();
		void Unbind();
};