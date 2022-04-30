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

class program;

/// @brief Level is primarily a data structure for complex 3D scenes
/// loads and manages geometry, textures and model matrices from some fbx file
class level
{
private:
	static constexpr auto vtx_stride = sizeof(vertex);

	uint32_t global_vertex_offset_ = 0;
	uint32_t global_index_offset_ = 0;

	GLuint vao_ = 0; // vertex layouts
	GLuint vbo_ = 0; // vertices
	GLuint ebo_ = 0; // elements
	GLuint ibo_ = 0; // indirect commands
	GLuint matrix_ssbo_ = 0; // transformations

	// mesh data - a loaded scene is entirely contained in these data structures
	std::vector<sub_mesh> meshes_; // contains mesh offsets for glDraw 
	std::vector<float> vertices; // contains a stream of vertices in (px,py,pz,ny,ny,nz,u,v)-form
	std::vector<unsigned int> indices_; // contains the indices that make triangles
	std::vector<Material> materials_; // contains all needed textures
	std::vector<render_item> render_queue_;
	hierarchy scene_graph_; // saves scene hierarchy and transformations
	hierarchy* dynamic_node_;
	std::vector<physics_mesh> rigid_;
	std::vector<physics_mesh> dynamic_;

	/// frustum culling
	std::shared_ptr<program> aabb_viewer_; // shader for debugging AABBs, toggle with F2
	std::shared_ptr<program> frustumviewer_; // shader for debugging AABBs, toggle with F2
	glm::mat4 cull_view_proj_;
	glm::vec4 frustum_planes_[6];
	glm::vec4 frustum_corners_[8];
	uint32_t models_loaded_ = 0;
	uint32_t models_visible_ = 0;
	double seconds_since_flush_ = 0;

	light_sources lights;

	std::shared_ptr<GlobalState> state_;
	PerFrameData* perframe_data_{};

	void load_meshes(const aiScene* scene);
	sub_mesh extract_mesh(const aiMesh* mesh);
	void generate_lods(std::vector<unsigned int>& indices, const std::vector<float>& vertices,
	                  std::vector<std::vector<unsigned int>>& LODs);
	bounding_box compute_bounds_of_mesh(const sub_mesh& mesh) const;
	void load_materials(const aiScene* scene);
	void traverse_tree(aiNode* n, hierarchy* parent, hierarchy* child);
	void load_shaders();
	void setup_vertex_buffers();
	void setup_draw_buffers();
	void load_lights(const aiScene* scene);
	static glm::mat4 to_glm_mat4(const aiMatrix4x4& mat);
	void build_render_queue(const hierarchy* node, glm::mat4 global_transform);
	uint32_t decide_lod(uint32_t lods, bounding_box aabb) const;
	void draw_aabbs(hierarchy node);
	void transform_bounding_boxes(hierarchy* node, glm::mat4 global_transform);
	void collect_rigid_physic_meshes(hierarchy* node, glm::mat4 global_transform);
	void collect_dynamic_physic_meshes(hierarchy* node, glm::mat4 global_transform);


	void reset_queue();
	void release();

public:
	level(const char* scene_path, std::shared_ptr<GlobalState> state, PerFrameData& perframe_data);
	~level() { release(); }

	void draw_scene();
	void draw_scene_shadow_map();


	std::vector<physics_mesh> get_rigid();
	std::vector<physics_mesh> get_dynamic();
	glm::mat4 get_tight_scene_frustum() const;
	light_sources* get_lights() { return &lights; }
};
