#pragma once

#include "EBO.h"
#include "VAO.h"
#include "Shader.h"
#include "Texture.h"
#include "Cubemap.h"


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
	Texture* diffuse; //todo: add default textures
	Texture* specular;
	Cubemap* cubemap;
	glm::vec4 coefficients;
	float reflection;

	void PrepareBuffer();
	void reverseIndices(); // changes face orientation

	// geometry constructors
	Mesh(float w, float h, float d);
	Mesh(float size);
	Mesh(int s, float h, float rad);
	Mesh(int longs, int lats, float rad);

public:
	glm::mat4 model = glm::mat4(1.0f);

	static Mesh Cube(float width, float height, float depth) {return  Mesh(width, height, depth);}
	static Mesh Cylinder(int segments, float height, float radius) { return  Mesh(segments, height, radius); }
	static Mesh Sphere(int longsegments, int latisegments, float radius) { return  Mesh(longsegments, latisegments, radius); }
	static Mesh Skybox(float size) { return  Mesh(size); }
	// todo obj loading

	glm::mat4 translate(glm::vec3 position);
	glm::mat4 rotate(float theta, glm::vec3 axis);

	void setMaterial(glm::vec4 coeff, float reflect);
	void setTextures(Texture* diff, Texture* spec, Cubemap* cube);
	void uploadMaterial(Shader shader);
	
	void Draw(Shader shader); // draws triangles
	
};