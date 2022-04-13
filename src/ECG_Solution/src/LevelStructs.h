#pragma once
#include <string>
#include <vector>
#include <glm/detail/type_vec.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#ifndef _LEVEL_STRUCTS_
#define _LEVEL_STRUCTS_

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
struct DrawElementsIndirectCommand
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
	BoundingBox(const glm::vec3& min, const glm::vec3& max) : min_(glm::min(min, max)), max_(glm::max(min, max)) {}
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

	BoundingBox nodeBounds;					// pretransformed bounds
	BoundingBox modelBounds;				// bounds in model space

	/// return TRS "model matrix" of the node
	glm::mat4 getNodeMatrix() const { return glm::translate(localTranslate) * glm::toMat4(localRotation) * glm::scale(localScale); }

	/// @brief set TRS "model matrix" of the node
	void setNodeMatrix(glm::mat4 M) { glm::decompose(M, localScale, localRotation, localTranslate, glm::vec3(), glm::vec4()); }
};

/// @brief contains a list of draw commands and matching model matrices for models of the same material
struct RenderItem
{
	std::string material;
	std::vector<DrawElementsIndirectCommand> commands;
	std::vector<glm::mat4> modelMatrices;
};

/// @brief contains single mesh for bullet physics simulation
struct PhysicsMesh
{
	std::vector<glm::vec3> vtxPositions;		// all positions (x,y,z) in model space
	glm::mat4 modelMatrix;						// model tranformation into world space
	std::shared_ptr<Hierarchy> node;			// pointer to set node matrices, only for dynamic objects
};

#endif