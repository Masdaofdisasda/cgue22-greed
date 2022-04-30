#pragma once
#include <string>
#include <vector>
#include <glm/detail/type_vec.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

/// @brief describes the position of a mesh in an index and vertex array 
struct sub_mesh
{
	std::string name;						// name of the mesh, for debugging
	std::vector<uint32_t> index_offset;		// start of mesh in vector indices_, [0] offset to original index - [8] offset to lowest LOD 
	uint32_t vertex_offset{};				// start of mesh in vector vertices
	std::vector<uint32_t> index_count;		// number of indices to render, [0] original index count - [8] lowest LOD
	uint32_t vertex_count{};				// number of vertices to render
	uint32_t material_index{};				// associated material
};

/// @brief describes one indirect command for glDraw_Indirect calls
struct draw_elements_indirect_command
{
	uint32_t count_;			// number of indices that get drawn, eg for single quad = 6
	uint32_t instanceCount_;	// number of instanced that get drawn, 0 means none, this programm doesn't use instanced rendering
	uint32_t firstIndex_;		// index offset, eg for LOD0 = 0, LOD1 = LOD0.idxcount, LOD1 = LOD0.idxcount + LOD1.idxcount, etc
	uint32_t baseVertex_;		// offset added before selecting vertices
	uint32_t baseInstance_;		// accessible in GLSL as "gl_BaseInstance", used for model matrix
};

/// @brief describes the bounding box of a mesh, can be used for frustum culling or physics simulation
struct bounding_box
{
	glm::vec3 min_;
	glm::vec3 max_;

	bounding_box() = default;
	bounding_box(const glm::vec3& min, const glm::vec3& max) : min_(glm::min(min, max)), max_(glm::max(min, max)) {}
};

/// @brief memory efficient TRS data capsule
struct transformation
{
	glm::vec3 translate;				
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4 get_matrix() const { return glm::translate(translate) * glm::toMat4(rotation) * glm::scale(scale); }
};

/// @brief a collection of settings for collectable items in the world
struct collectable_item_properties {
	float worth = 100;
	float weight = 1;
};

/// @brief a collection of settings for an object in the game world
struct game_properties {
	std::string display_name = "Gameobject";
	bool is_active = true; // only active items are rendered. only active items are allowed to interact with the physics world
	bool is_collectable = false; // determines if the player can collect it
	bool is_ground = true; // determines wether the player can jump off of it
	collectable_item_properties collectableItemProperties; // if the gamobject is collectable this determines some extra properties
};

/// @brief implements a simple scene graph of hierarchical transformations
struct hierarchy
{
	std::string name;
	hierarchy* parent = nullptr;
	std::vector <hierarchy> children;
	std::vector<uint32_t> model_indices;		// assumed to only hold one model

	transformation TRS;

	bounding_box node_bounds;					// pretransformed bounds
	bounding_box model_bounds;					// bounds in model space

	game_properties game_properties;

	/// return TRS "model matrix" of the node
	glm::mat4 get_node_matrix() const { return TRS.get_matrix(); }

	/// @return memory efficient TRS data
	transformation get_node_trs() const { return TRS; }

	/// @brief set TRS "model matrix" of the node
	void set_node_trs(const glm::vec3 T, const glm::quat R, const glm::vec3 S) { TRS.translate = T; TRS.rotation = R; TRS.scale = S; }
};

/// @brief contains a list of draw commands and matching model matrices for models of the same material
struct render_item
{
	std::string material;
	std::vector<draw_elements_indirect_command> commands;
	std::vector<glm::mat4> model_matrices;
};

/// @brief contains single mesh for bullet physics simulation
struct physics_mesh
{
	std::vector<float> vtx_positions;		// all positions (x,y,z) in model space
	transformation model_trs;				// model tranformation into world space
	hierarchy* node{};						// pointer to set node matrices, only for dynamic objects
};

/// @brief needed for mesh optimizer
struct vertex
{
	float px, py, pz;
	float nx, ny, nz;
	float tx, ty;
};
