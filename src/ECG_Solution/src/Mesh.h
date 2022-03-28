#pragma once

#include "EBO.h"
#include "VBO.h"
#include "Material.h"


// class for meshes containing vertices(
class Mesh
{
private:

	// VAO of the mesh
	GLuint vao_ID = 0;

	// contains vertices with position and color (vector for variable length) 
	std::vector <Vertex> vertices; // VBO
	// contains connected vertices
	std::vector <GLuint> indices; // EBO

	// Texture & Material
	Material* material = nullptr;


	void PrepareBuffer();	// setup VAO
	void reverseIndices(); // changes face orientation
	void Mesh::LinkAttrib(GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset);

	// skybox constructor
	Mesh(float size);

	void Release()
	{
		glDeleteBuffers(1, &vao_ID);
		vao_ID = 0;
	}

public:
	glm::mat4 model = glm::mat4(1.0f);

	static Mesh Skybox() { return  Mesh(1.0f); }
	Mesh(const char* fileName, Material* mat);
	Mesh();

	void setMatrix(glm::vec3 translate, float degree, glm::vec3 axis, glm::vec3 scale);

	// ensure RAII compliance
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&& other) noexcept : vao_ID(other.vao_ID)
	{
		other.vao_ID = 0; //Use the "null" ID for the old object.
	}

	Mesh& operator=(Mesh&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(vao_ID, other.vao_ID);
		}
	}

	Material* getMaterial();
	int getIndicesSize();
	void BindVAO();

	~Mesh() { Release(); }
};