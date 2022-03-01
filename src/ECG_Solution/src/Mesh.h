#pragma once

#include "EBO.h"
#include "VAO.h"
#include "shaderClass.h"


// primitive mesh objects (box, cylinder, sphere, torus)
class Mesh
{
public:

	// VAO of the mesh
	VAO VAO;

	// contains vertices with position and color (vector for variable length) 
	std::vector <Vertex> vertices; // VBO
	// contains connected vertices
	std::vector <GLuint> indices; // EBO

	glm::mat4 model = glm::mat4(1.0f);

	// material
	GLuint texture, specular, cubemap;
	glm::vec4 coefficients;
	float reflection;


	// box geometry constructor
	// width, height, depth, red, green, blue
	Mesh(float w, float h, float d, float r, float g, float b);

	// skybox geometry constructor
	Mesh(float size);

	// cylinder geometry constructor
	// segments, height, radius, red, green, blue
	Mesh(int s, float h, float rad, float r, float g, float b);

	// sphere geometry constructor
	// longitude segments, latitude segments, radius, red, green, blue
	Mesh(int longs, int lats, float rad, float r, float g, float b);

	glm::mat4 translate(glm::vec3 position);
	glm::mat4 rotate(float theta, glm::vec3 axis);

	void setMaterial(const char* texFile, const char* specFile, const char* cubeFile, glm::vec4 coeff, float reflect);
	void uploadMaterial(Shader shader);
	
	void Draw(Shader shader); // draws triangles
	//~Mesh(); // deletes Mesh
	
private:
	void PrepareBuffer();
	void reverseIndices(); // if wrong face is drawn
	void loadTexture(const char* tex, const char* spec, const char* cube);
};