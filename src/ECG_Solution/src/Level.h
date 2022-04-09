#pragma once
#include <vector>
#include <functional>
#include "Material.h"
#include "LightSource.h"
#include "Camera.h"
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>

/// @brief describes the position of a mesh in an index and vertex array 
struct subMesh
{
	std::string name;			// name of the mesh, for debugging
	uint32_t indexOffset;		// start of mesh in vector indices
	uint32_t vertexOffset;		// start of mesh in vector vertices
	uint32_t indexCount;		// number of indices to render
	uint32_t vertexCount;		// number of vertices to render
	uint32_t materialIndex;		// associated material
};

/// @brief deascribes one indirect command for GlDraw_Indrect calls
struct DrawElementsIndirectCommand		// TODO
{
	uint32_t count_;
	uint32_t instanceCount_;
	uint32_t first_;
	uint32_t baseInstance_;
};

/// @brief describes the bounding box of a mesh, can be used for frustum culling or physics simlution
struct BoundingBox
{
	glm::vec3 min_;
	glm::vec3 max_;

	BoundingBox() = default;
	BoundingBox(const glm::vec3 & min, const glm::vec3 & max) : min_(glm::min(min, max)), max_(glm::max(min, max)) {}
};

/// @brief implements a simple scene graph for hierarchical tranforamtions
struct Hierarchy
{
	std::string name;
	Hierarchy* parent = nullptr;			// parent node
	std::vector <Hierarchy> children;		// children nodes
	std::vector<uint32_t> modelIndices;		// models in this node

	glm::vec3 localTranslate;				// local transformation
	glm::quat localRotation;
	glm::vec3 localScale;

	BoundingBox nodeBounds;					// bounds of the nodes underlying children
	std::vector<BoundingBox> modelBounds;	// bounds of the models;

	/// return TRS "model matrix" of the node
	glm::mat4 getNodeMatrix() const { return glm::translate(localTranslate) * glm::toMat4(localRotation) * glm::scale(localScale); }

	/// @brief set TRS "model matrix" of the node
	void setNodeMatrix(glm::mat4 M) { glm::decompose(M , localScale, localRotation, localTranslate, glm::vec3(), glm::vec4());}
};

/// @brief contains a list of draw commands and matching model matrices for models of the same material
struct RenderItem
{
	std::string material;
	std::vector<DrawElementsIndirectCommand> commands;
	std::vector<glm::mat4> modelMatrices;
};

//---------------------------------------------------------------------------------------------------------------//

/// @brief Level is primarily a data structure for complex 3D scenes
/// loads and manages geometry, textures and model matrices from some fbx file
class Level {
private:
	uint32_t globalVertexOffset = 0;
	uint32_t globalIndexOffset = 0;
	uint32_t boundMaterial = -1;

	GLuint VAO = 0; // vertex layouts
	GLuint VBO = 0; // vertices
	GLuint EBO = 0; // elements
	GLuint IBO = 0; // indirect commands
	GLuint matrixSSBO = 0; // tranformations

	// mesh data - a loaded scene is entirely contained in these data structures
	std::vector <subMesh> meshes;			// contains mesh offsets for glDraw 
	std::vector<float> vertices;			// contains a stream of vertices in (px,py,pz,ny,ny,nz,u,v)-form
	std::vector <GLuint> indices;			// contains the indices that make triangles
	std::vector <Material> materials;		// contains all needed textures
	std::vector <BoundingBox> boxes;		// contains all bounding boxes of the meshes
	std::vector<RenderItem> renderQueue;	// contains for every material render commands and matrices
	Hierarchy sceneGraph;					// saves scene hierarchy and transformations
	Hierarchy* rigid;						// parent node of all rigid meshes (ground, walls, ...)
	Hierarchy* dynamic;						// parent node of all dynamic meshes (items, ...)

	LightSources lights;

	subMesh extractMesh(const aiMesh* mesh);
	BoundingBox computeBoundsOfMesh(subMesh mesh);
	BoundingBox computeBoundsOfNode(std::vector <Hierarchy> children, std::vector<BoundingBox> modelBounds);
	Material Level::loadMaterials(const aiMaterial* M);
	void traverseTree(aiNode* n, Hierarchy* parent, Hierarchy* child);
	void setupVertexBuffers();
	void setupDrawBuffers();
	void loadLights(const aiScene* scene);
	glm::mat4 toGlmMat4(const aiMatrix4x4& mat);
	void buildRenderQueue(const Hierarchy* node, glm::mat4 globalTransform);

	void resetQueue();
	void Release();
	
public:
	Level(const char* scenePath);
	~Level() { Release(); }

	void DrawGraph();

	Hierarchy* getRigidNodes() { return rigid; }
	Hierarchy* getDynamicNodes() { return dynamic; }
	LightSources* getLights() { return &lights; }
	std::vector <DirectionalLight> getDirectionalLights() { return lights.directional; }
	std::vector <PositionalLight> getPointLights() { return lights.point; }
};