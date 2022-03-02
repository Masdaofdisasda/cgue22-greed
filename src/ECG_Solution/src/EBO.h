#pragma once
#include"Utils.h"
#include <vector>

class EBO
{
	private:

		// Reference ID of the EBO
		GLuint ebo_ID = 0;

		void Release()
		{
			glDeleteBuffers(1, &ebo_ID);
			ebo_ID = 0;
		}

	public:

		// creates an Element Buffer Object from indices
		EBO(std::vector<GLuint>& indices);

		~EBO() { Release(); }

		// ensure RAII compliance
		EBO(const EBO&) = delete;
		EBO& operator=(const EBO&) = delete;

		EBO(EBO&& other) : ebo_ID(other.ebo_ID)
		{
			other.ebo_ID = 0; //Use the "null" ID for the old object.
		}

		EBO& operator=(EBO&& other)
		{
			//ALWAYS check for self-assignment.
			if (this != &other)
			{
				Release();
				//obj_ is now 0.
				std::swap(ebo_ID, other.ebo_ID);
			}
		}

		void Bind();
		void Unbind();
};