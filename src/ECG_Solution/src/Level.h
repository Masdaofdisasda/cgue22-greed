#pragma once
#include <vector>
#include <functional>
#include "Mesh.h"
#include "LightSource.h"
#include "Camera.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <unordered_map>

/*
Class for all Level classes.
Helps with seperating different scenes with different models, lights, cameras and controls.
Level data can be read from a file.
*/

struct drawData		// data for drawing
{
	uint32_t meshIndex;			// specify mesh in vector meshes
	uint32_t materialIndex;		// specify material in vector ?
	uint32_t transformIndex;	// specify model tranformation in vector ?
};

struct MeshObj		// mesh object
{
	uint32_t indexOffset;		// start of mesh in vector indices
	uint32_t vertexOffset;		// start of mesh in vector vertices
	uint32_t indexCount;		// number of indices to render
	uint32_t vertexCount;		// number of vertices to render
	uint32_t materialIndex;		// associated material
};

struct DrawElementsIndirectCommand		// TODO
{
	GLuint count_;
	GLuint instanceCount_;
	GLuint firstIndex_;
	GLuint baseVertex_;
	GLuint baseInstance_;
};

struct BoundingBox		// might be used for frustum culling
{
	glm::vec3 min_;
	glm::vec3 max_;

	BoundingBox() = default;
	BoundingBox(const glm::vec3 & min, const glm::vec3 & max) : min_(glm::min(min, max)), max_(glm::max(min, max)) {}
};


//---------------------------------------------------------------------------------------------------------------//
class Level {
private:
	uint32_t globalVertexOffset = 0;
	uint32_t globalIndexOffset = 0;

	GLuint VAO = 0; // vertex layouts
	GLuint VBO = 0; // vertices
	GLuint EBO = 0; // elements
	GLuint IBO = 0; // indirect commands
	GLuint matrixSSBO = 0; // tranformations
	GLuint materialSSBO = 0; // textures

	// mesh data - a loaded scene is entirely contained in these data structures
	std::vector <MeshObj> meshes;			// contains mesh offsets for glDraw 
	std::vector <drawData> models;			// describes a single model
	std::vector<float> vertices;			// contains a stream of vertices in (px,py,pz,ny,ny,nz,u,v)-form
	std::vector <GLuint> indices;			// contains the indices that make triangles
	std::vector <glm::mat4> transforms;		// contains all tranformation matrices
	std::vector <Material> materials;		// contains all needed textures
	std::vector <BoundingBox> boxes;		// contains all bounding boxes of the meshes
	std::vector<DrawElementsIndirectCommand> drawCommands_; //TODO

	LightSources lights;

	MeshObj extractMesh(const aiMesh* mesh);
	void calculateBoundingBoxes();
	Material Level::loadMaterials(const aiMaterial* M);
	void setupVertexBuffers();
	void setupDrawBuffers();
	void loadLights();
	
public:
	Level(const char* scenePath);
	~Level() {}

	void Draw() const;

	LightSources getLights() { return lights; }

	std::vector <DirectionalLight> getDirectionalLights() { return lights.directional; }
	std::vector <PositionalLight> getPointLights() { return lights.point; }
};