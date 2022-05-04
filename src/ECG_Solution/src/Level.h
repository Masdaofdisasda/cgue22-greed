#pragma once
#include <vector>
#include <functional>
#include "Material.h"
#include "LightSource.h"
#include "Camera.h"
#include "LevelStructs.h"
#include "FrustumCuller.h"
#include "LodSystem.h"
#include "buffer.h"
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

	// buffers
	GLuint vao_ = 0;
	buffer ibo_{ GL_DRAW_INDIRECT_BUFFER };
	buffer ssbo_{ GL_SHADER_STORAGE_BUFFER };

	// mesh data - a loaded scene is entirely contained in these data structures
	std::vector<sub_mesh> meshes_; 
	std::vector<float> vertices; 
	std::vector<unsigned int> indices_; 
	std::vector<Material> materials_;
	light_sources lights_;
	std::vector<render_item> render_queue_shadow_;
	std::vector<render_item> render_queue_scene_;
	hierarchy scene_graph_;
	hierarchy* lava_;
	hierarchy* dynamic_node_;
	std::vector<physics_mesh> rigid_;
	std::vector<physics_mesh> dynamic_;

	/// frustum culling
	std::unique_ptr<program> aabb_viewer_; 
	std::unique_ptr<program> frustumviewer_;

	std::shared_ptr<global_state> state_;
	PerFrameData* perframe_data_{};

	/**
	 * \brief iterates through a list of assimp meshes, parses adds them to the meshes array
	 * \param scene the loaded scene containing the meshes to load
	 */
	void load_meshes(const aiScene* scene);

	/**
	 * \brief extracts vertex and index data, optmizes them and adds them to the indices/vertices arrays
	 * \param mesh a mesh containing geometry data
	 * \return a optimized mesh with all data needed for rendering
	 */
	sub_mesh extract_mesh(const aiMesh* mesh);

	/**
	 * \brief generates up to 8 LODs for a mesh, code from the 3D Rendering cookbook
	 * \param indices optimized indices for a mesh
	 * \param vertices optimized vertices for a mesh (1 vertex consists of 8 floats)
	 * \param LODs the target index array where the LODs get copied to
	 */
	void generate_lods(std::vector<unsigned int>& indices, const std::vector<float>& vertices,
	                   std::vector<std::vector<unsigned int>>& LODs);

	/**
	 * \brief finds the maximum and minimum vertex positions of a mesh, which should define the bounds (AABB)
	 * \param mesh for finding the AABB
	 * \return the min/max point of the AABB of the mesh
	 */
	bounding_box compute_bounds_of_mesh(const sub_mesh& mesh) const;

	/**
	 * \brief loads all materials (textures) from the material list assimp provides
	 * \param scene scene contains the pointer to the material list
	 */
	void load_materials(const aiScene* scene);

	/**
	 * \brief recursive function that builds a scenegraph with hierarchical transformation, similiar to assimps scene
	 * \param n is an assimp node that holds transformations, nodes or meshes
	 * \param parent is the parent node of the currently created node, mainly used for debugging
	 * \param node  is the current node from the view of the parent node
	 */
	void traverse_tree(const aiNode* n, hierarchy* parent, hierarchy* node);

	/**
	 * \brief loads and compiles shaders for debugging the AABBs and the frustum culler
	 */
	void load_shaders();

	/**
	 * \brief Creates and fills vertex and index buffers and sets up the "big" vao which contains all meshes
	 */
	void setup_buffers();

	/**
	 * \brief loads all direction/point lights from the assimp scene, corrects position and rotation automatically
	 * \param scene is the scene containing the lights and root node
	 */
	void load_lights(const aiScene* scene);

	/**
	 * \brief simple helper function for converting the assimp 4x4 matrices to 4x4 glm matrices
	 * \param mat is a 4x4 matrix from assimp
	 * \return 4x4 matrix in glm format
	 */
	static glm::mat4 to_glm_mat4(const aiMatrix4x4& mat);

	/**
	 * \brief recursively builds for every material a render command list by adding all unculled objects
	 * \param node that gets checked for models/culling
	 * \param global_transform is the accumulation of parent transforms
	 * \param high_quality only uses lowest LOD if false, uses normal LOD decision heuristic if true
	 */
	void build_render_queue(const hierarchy* node, glm::mat4 global_transform);


	/**
	 * \brief recursively renders every AABB as a wireframe box
	 * \param node that gets traversed
	 */
	void draw_aabbs(hierarchy node);

	/**
	 * \brief recursively transforms AABBs in the scene graph from model space to world space
	 * \param node that gets transformed
	 * \param global_transform is the accumulation of parent transforms
	 */
	void transform_bounding_boxes(hierarchy* node, glm::mat4 global_transform);

	/**
	 * \brief adds position vertex data of all "rigid" children to the rigid_ list
	 * \param node that gets added if it has data
	 * \param global_transform is the accumulation of parent transforms
	 */
	void collect_rigid_physic_meshes(hierarchy* node, glm::mat4 global_transform);

	/**
	 * \brief adds position vertex data of all "dynamic" children to the dynamic_ list
	 * \param node that gets added if it has data
	 * \param global_transform is the accumulation of parent transforms
	 */
	void collect_dynamic_physic_meshes(hierarchy* node, glm::mat4 global_transform);

	/**
	 * \brief deletes all entries in the render queue, call after a draw cycle
	 */
	void reset_queue();

	/**
	 * \brief release all resources, buffers and textures
	 */
	void release() const;

public:
	/// @brief loads an fbx file from the given path and converts it to GL data structures
	/// @param scene_path location of the fbx file, expected to be in folder "assets"
	/// @param state global state of the program, needed for screen resolution, etc
	/// @param perframe_data camera uniforms, needed for frustum culling
	level(const char* scene_path, std::shared_ptr<global_state> state, PerFrameData& perframe_data);
	~level() { release(); }

	/**
	 * \brief sets up indirect render calls, binds the data and calls the actual draw routine
	 * it is assumed that draw_scene_shadow_map was called prior and no other vao was bound
	 */
	void draw_scene();

	/**
	 * \brief same as draw_scene, but nothing gets culled and no textures are bound
	 */
	void draw_scene_shadow_map();

	/**
	 * \brief generates a vector of rigid meshes, which are unmovable
	 * \return that vector
	 */
	std::vector<physics_mesh> get_rigid();

	/**
	 * \brief generates a vector of dynamic meshes, which are movable
	 * \return that vector
	 */
	std::vector<physics_mesh> get_dynamic();

	/** TODO
	 * \brief calculates the tightest possible orthogonal view frustum of the whole scene, used for directional shadow mapping
	 * \return an orthogonal projection of the level
	 */
	glm::mat4 get_tight_scene_frustum(glm::mat4 light_view) const;
	
	light_sources* get_lights() { return &lights_; }
};
