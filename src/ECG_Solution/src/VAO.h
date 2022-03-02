#pragma once
#include"Utils.h"
#include"VBO.h"

class VAO
{
	private:

		// Reference ID of the VAO
		GLuint vao_ID = 0;

		void Release()
		{
			glDeleteBuffers(1, &vao_ID);
			vao_ID = 0;
		}

	public:	

		// creates Vertex Arrays Object
		VAO();

		~VAO() { Release(); }
		
		// ensure RAII compliance
		VAO(const VAO&) = delete;
		VAO& operator=(const VAO&) = delete;

		VAO(VAO&& other) : vao_ID(other.vao_ID)
		{
			other.vao_ID = 0; //Use the "null" ID for the old object.
		}

		VAO& operator=(VAO&& other)
		{
			//ALWAYS check for self-assignment.
			if (this != &other)
			{
				Release();
				//obj_ is now 0.
				std::swap(vao_ID, other.vao_ID);
			}
		}

		// links an VBO color and position to an VAO
		void LinkAttrib(VBO* VBO, GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset);
		void Bind();
		void Unbind();
};