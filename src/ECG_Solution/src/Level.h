#pragma once
#include <vector>
#include <functional>
#include "Material.h"
#include "LightSource.h"
#include "Camera.h"
#include "LevelStructs.h"
#include "FrustumCulling.h"
#include <glm/gtx/matrix_decompose.hpp> 
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>


//---------------------------------------------------------------------------------------------------------------//

#ifndef _LEVEL_
#define _LEVEL_
class Program;
/// @brief Level is primarily a data structure for complex 3D scenes
/// loads and manages geometry, textures and model matrices from some fbx file
class Level {
private:
	uint32_t globalVertexOffset = 0;
	uint32_t globalIndexOffset = 0;

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

	/// frustum culling
	std::shared_ptr<Program> AABBviewer;	// shader for debugging AABBs, toggle with F2
	std::shared_ptr<Program> Frustumviewer;	// shader for debugging AABBs, toggle with F2
	glm::mat4 cullViewProj;
	glm::vec4 frustumPlanes[6];
	glm::vec4 frustumCorners[8];
	uint32_t ModelsLoaded = 0;
	uint32_t ModelsVisible = 0;
	double secondsSinceFlush = 0;

	LightSources lights;

	GlobalState* globalState;
	PerFrameData* perframeData;

	subMesh extractMesh(const aiMesh* mesh);
	BoundingBox computeBoundsOfMesh(subMesh mesh);
	Material Level::loadMaterials(const aiMaterial* M);
	void traverseTree(aiNode* n, Hierarchy* parent, Hierarchy* child);
	void setupVertexBuffers();
	void setupDrawBuffers();
	void loadLights(const aiScene* scene);
	glm::mat4 toGlmMat4(const aiMatrix4x4& mat);
	void buildRenderQueue(const Hierarchy* node, glm::mat4 globalTransform);
	void DrawAABBs(Hierarchy node);
	void transformBoundingBoxes(Hierarchy* node, glm::mat4 globalTransform);

	void resetQueue();
	void Release();
	
public:
	Level(const char* scenePath, GlobalState& state, PerFrameData& pfdata);
	~Level() { Release(); }

	void DrawGraph();

	Hierarchy* getRigidNodes() { return rigid; }
	Hierarchy* getDynamicNodes() { return dynamic; }
	LightSources* getLights() { return &lights; }
	std::vector <DirectionalLight> getDirectionalLights() { return lights.directional; }
	std::vector <PositionalLight> getPointLights() { return lights.point; }
};

#endif