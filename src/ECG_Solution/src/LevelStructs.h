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
	uint32_t firstIndex_;
	uint32_t baseVertex_;
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

/// @brief memory efficient TRS data capsule
struct Transformation
{
	glm::vec3 Translate;				
	glm::quat Rotation;
	glm::vec3 Scale;

	glm::mat4 getMatrix() const { return glm::translate(Translate) * glm::toMat4(Rotation) * glm::scale(Scale); }
};

/// @brief implements a simple scene graph for hierarchical tranforamtions
struct Hierarchy
{
	std::string name;
	Hierarchy* parent = nullptr;			// parent node
	std::vector <Hierarchy> children;		// children nodes
	std::vector<uint32_t> modelIndices;		// models in this node

	Transformation TRS;

	BoundingBox nodeBounds;					// pretransformed bounds
	BoundingBox modelBounds;				// bounds in model space

	/// return TRS "model matrix" of the node
	glm::mat4 getNodeMatrix() const { return TRS.getMatrix(); }

	/// @return memory efficient TRS data
	Transformation getNodeTRS() const { return TRS; }

	/// @brief set TRS "model matrix" of the node
	void setNodeTRS(glm::vec3 T, glm::quat R, glm::vec3 S) { TRS.Translate = T; TRS.Rotation = R; TRS.Scale = S; }
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
	std::vector<float> vtxPositions;		// all positions (x,y,z) in model space
	Transformation modelTRS;						// model tranformation into world space
	Hierarchy* node;			// pointer to set node matrices, only for dynamic objects
};

#endif