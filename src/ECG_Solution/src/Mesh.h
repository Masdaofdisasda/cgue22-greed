#pragma once

#include "EBO.h"
#include "VAO.h"
#include "Material.h"


// primitive mesh objects (box, cylinder, sphere, torus)
class Mesh
{
private:

	// VAO of the mesh
	VAO VAO;

	// contains vertices with position and color (vector for variable length) 
	std::vector <Vertex> vertices; // VBO
	// contains connected vertices
	std::vector <GLuint> indices; // EBO

	// Texture & Material
	Material* material;


	void PrepareBuffer();
	void reverseIndices(); // changes face orientation

	// geometry constructors
	Mesh(float w, float h, float d, Material* mat);
	Mesh(float size, Material* mat);
	Mesh(int s, float h, float rad, Material* mat);
	Mesh(int longs, int lats, float rad, Material* mat);

public:
	glm::mat4 model = glm::mat4(1.0f);

	static Mesh Cube(float width, float height, float depth, Material* mat) {return  Mesh(width, height, depth, mat);}
	static Mesh Cylinder(int segments, float height, float radius, Material* mat) { return  Mesh(segments, height, radius, mat); }
	static Mesh Sphere(int longsegments, int latisegments, float radius, Material* mat) { return  Mesh(longsegments, latisegments, radius, mat); }
	static Mesh Skybox(float size, Material* mat) { return  Mesh(size, mat); }
	Mesh(const char* fileName, Material* mat);

	glm::mat4 translate(glm::vec3 position);
	glm::mat4 rotate(float theta, glm::vec3 axis);

	Material* getMaterial();
	int getIndicesSize();
	void BindVAO();
	
};