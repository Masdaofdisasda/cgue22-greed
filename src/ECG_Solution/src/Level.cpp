#include "Level.h"
#include "Program.h"
#include <meshoptimizer/meshoptimizer.h>
#include <unordered_map>
#include <thread>
#include <optick/optick.h>

level::level(const char* scene_path, const std::shared_ptr<global_state> state, PerFrameData& perframe_data)
: state_(state), perframe_data_(&perframe_data)
{
	std::cout << "import scene from fbx file..." << std::endl;
	OPTICK_PUSH("parse fbx file")
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(scene_path,
	                                         aiProcess_RemoveRedundantMaterials |
	                                         aiProcess_FindInvalidData |
	                                         aiProcess_FlipUVs |
	                                         aiProcess_ValidateDataStructure | 
	                                         0);

	if (!&scene){
		std::cerr << "ERROR: Couldn't load scene" << std::endl;
		exit(EXIT_FAILURE);
	}
	OPTICK_POP()

	OPTICK_PUSH("load meshes")
	std::thread mesh(&level::load_meshes, this, std::ref(scene));
	std::thread light(&level::load_lights, this, std::ref(scene));

	OPTICK_PUSH("load materials")
	load_materials(scene);
	OPTICK_POP()

	// build scene graph and calculate AABBs
	std::cout << "build scene hierarchy..." << std::endl;
	mesh.join();
	OPTICK_POP()
	OPTICK_PUSH("build scene graph")
	traverse_tree(scene->mRootNode, glm::mat4(1), lava);
	transform_bounding_boxes();
	get_scene_bounds();
	collect_physic_meshes();
	build_render_queue();
	assert(queue_scene_.commands.size() == scene_.size());
	OPTICK_POP()

	OPTICK_PUSH("setup level buffers")
	setup_buffers();
	OPTICK_POP()

	// finalize
	light.join();
	load_shaders();

	std::cout << std::endl; // debug breakpoint
}

void level::load_meshes(const aiScene* scene)
{
	global_vertex_offset_ = 0;
	global_index_offset_ = 0;

	meshes_.reserve(scene->mNumMeshes);

	std::cout << "loading meshes..." << std::endl;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		meshes_.push_back(extract_mesh(mesh));
	}
	frustum_culler::models_loaded = meshes_.size();
}

sub_mesh level::extract_mesh(const aiMesh* mesh)
{

	printf("Mesh [%s] %u\n", mesh->mName.C_Str(), meshes_.size() + 1);
	sub_mesh m;
	m.name = mesh->mName.C_Str();
	m.vertex_offset = global_vertex_offset_;
	m.material_index = mesh->mMaterialIndex;
			
	std::vector<vertex> raw_vertices;
	std::vector <unsigned int> raw_indices;			

	// extract vertices from the aimesh
	for (size_t j = 0; j < mesh->mNumVertices; j++)
	{
		const aiVector3D p = mesh->HasPositions() ? mesh->mVertices[j] : aiVector3D(0.0f);
		const aiVector3D n = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f, 1.0f, 0.0f);
		const aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.5f, 0.5f, 0.0f);

		vertex vtx =
		{
			p.x, p.y, p.z,
			n.x, n.y, n.z,
			t.x, t.y
		};

		raw_vertices.push_back(vtx);
	}

	//extract indices from the aimesh
	for (size_t j = 0; j < mesh->mNumFaces; j++)
	{
		for (unsigned k = 0; k != mesh->mFaces[j].mNumIndices; k++)
		{
			GLuint index = mesh->mFaces[j].mIndices[k];
			raw_indices.push_back(index);
		}
	}

	// re-index geometry
	std::vector<unsigned int> remap(raw_indices.size());
	size_t vertex_count = meshopt_generateVertexRemap(remap.data(), raw_indices.data(), raw_indices.size(), raw_vertices.data(), raw_indices.size(), vtx_stride);

	std::vector <unsigned int> opt_indices(raw_indices.size());
	std::vector<vertex> opt_vertices(vertex_count);

	meshopt_remapIndexBuffer(opt_indices.data(), raw_indices.data(), raw_indices.size(), remap.data());
	meshopt_remapVertexBuffer(opt_vertices.data(), raw_vertices.data(), raw_vertices.size(), vtx_stride, remap.data());

	// further optimize geometry
	meshopt_optimizeVertexCache(opt_indices.data(), opt_indices.data(), raw_indices.size(), vertex_count);
	meshopt_optimizeOverdraw(opt_indices.data(), opt_indices.data(), raw_indices.size(), &opt_vertices[0].px, vertex_count, vtx_stride, 1.05f);
	meshopt_optimizeVertexFetch(opt_vertices.data(), opt_indices.data(), raw_indices.size(), opt_vertices.data(), vertex_count, vtx_stride);

	m.vertex_count = opt_vertices.size();

	std::vector<float> result_vertices;
	for (const auto& vertex : opt_vertices)
	{
		result_vertices.push_back(vertex.px); result_vertices.push_back(vertex.py); result_vertices.push_back(vertex.pz);
		result_vertices.push_back(vertex.nx); result_vertices.push_back(vertex.ny); result_vertices.push_back(vertex.nz);
		result_vertices.push_back(vertex.tx); result_vertices.push_back(vertex.ty);
	}

	std::vector<std::vector<unsigned int>> LODs;
	generate_lods(opt_indices, result_vertices, LODs);

	vertices.insert(vertices.end(), result_vertices.begin(), result_vertices.end());

	auto index_sum = 0;
	for (auto& LOD : LODs)
	{
		m.index_count.push_back(LOD.size());
		m.index_offset.push_back(global_index_offset_ + index_sum);
		index_sum += LOD.size();
		indices_.insert(indices_.end(), LOD.begin(), LOD.end());
	}

	global_vertex_offset_ += m.vertex_count;
	global_index_offset_ += index_sum;
	return m;
}

void level::generate_lods(std::vector<unsigned int>& indices,const std::vector<float>& vertices, std::vector<std::vector<unsigned int>>& LODs)
{
	const size_t vertices_count_in = vertices.size() / 8;
	size_t target_indices_count = indices.size();

	uint8_t lod = 1;

#ifdef _DEBUG
	printf("LOD0: %i indices   \n", static_cast<int>(indices.size()));
#endif

	LODs.push_back(indices);

	constexpr auto target = 1024; // for testing, final should be 1024
	while (target_indices_count > target && lod < 8)
	{
		target_indices_count = indices.size() / 2;

		bool sloppy = false;

		size_t num_opt_indices = meshopt_simplify(
			indices.data(),
			indices.data(), (unsigned int)indices.size(),
			vertices.data(), vertices_count_in,
			sizeof(float) * 8,
			target_indices_count, 0.02f);

		// cannot simplify further
		if (static_cast<size_t>(num_opt_indices * 1.1f) > indices.size())
		{
			if (lod > 1)
			{
				// try harder
				num_opt_indices = meshopt_simplifySloppy(
					indices.data(),
					indices.data(), indices.size(),
					vertices.data(), vertices_count_in,
					sizeof(float) * 8,
					target_indices_count);
				sloppy = true;
				if (num_opt_indices == indices.size()) break;
			}
			else
				break;
		}

		indices.resize(num_opt_indices);

		meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertices_count_in);

#ifdef _DEBUG
		printf("LOD%i: %i indices %s   \n", static_cast<int>(lod), static_cast<int>(num_opt_indices), sloppy ? "[sloppy]" : "");
#endif

		lod++;

		LODs.push_back(indices);
	}
}

bounding_box level::compute_bounds_of_mesh(const sub_mesh& mesh) const
{
	const auto num_indices = mesh.vertex_count;

	glm::vec3 vmin(std::numeric_limits<float>::max());
	glm::vec3 vmax(std::numeric_limits<float>::lowest());

	for (auto i = 0; i != num_indices; i++)
	{
		const auto vertex_offset = (mesh.vertex_offset + i) * 8;
		const float* vf = &vertices[vertex_offset];

		vmin = glm::min(vmin, glm::vec3(vf[0], vf[1], vf[2]));
		vmax = glm::max(vmax, glm::vec3(vf[0], vf[1], vf[2]));
	}
	return {vmin, vmax };
}

void level::transform_bounding_boxes() const
{
	for (const entity& entity : scene_)
	{
		glm::mat4 M = entity.get_node_matrix();
		bounding_box bounds = entity.model_bounds;
		bounds.min_ = M * glm::vec4(bounds.min_, 1.0f);
		bounds.max_ = M * glm::vec4(bounds.max_, 1.0f);
		entity.world_bounds = bounding_box(bounds.min_, bounds.max_);
	}
}

void level::get_scene_bounds()
{
	glm::vec3 vmin(std::numeric_limits<float>::max());
	glm::vec3 vmax(std::numeric_limits<float>::lowest());

	for (entity entity : scene_)
	{
		glm::vec3 cmin = entity.world_bounds.min_;
		glm::vec3 cmax = entity.world_bounds.max_;

		vmin = glm::min(vmin, cmin);
		vmax = glm::max(vmax, cmax);
	}

	scene_bounds_ = bounding_box(vmin, vmax);
}

void level::load_materials(const aiScene* scene)
{
	std::cout << "loading materials..." << std::endl;

	for (size_t m = 0; m < scene->mNumMaterials; m++)
	{
		const aiMaterial* mm = scene->mMaterials[m];

		printf("Material [%s] %u\n", mm->GetName().C_Str(), m + 1);

		aiString path;

		if (aiGetMaterialTexture(mm, aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS)
		{
			const auto albedo_map = std::string(path.C_Str());
		}

		//	all other materials can be found with: aiTextureType_NORMAL_CAMERA/_METALNESS/_DIFFUSE_ROUGHNESS/_AMBIENT_OCCLUSION

		if(path.length != 0)
		{
			material mat;
			material::create(path.C_Str(), mm->GetName().C_Str(), mat);
			materials_.push_back(mat);
		} else //default
		{
			material mat;
			mat.type = invisible;
			//create("textures/default/albedo.jpg", "default", mat);
			materials_.push_back(mat);
		}

		if (strcmp(mm->GetName().C_Str(),"Lava_1") == 0)
		{
			perframe_data_->normal_map.y = static_cast<float>(materials_.size()-1);
		}
	}
}

void level::traverse_tree(const aiNode* n, const glm::mat4 mat, entity_type type)
{
	if (strcmp(n->mName.C_Str(),"Rigid") == 0) type = rigid;
	if (strcmp(n->mName.C_Str(), "Dynamic") == 0) type = dynamic;
	if (strcmp(n->mName.C_Str(), "Deco") == 0) type = decoration;
	if (strcmp(n->mName.C_Str(), "Lava1") == 0) lava_ = scene_.size();

	glm::mat4 M = mat * to_glm_mat4(n->mTransformation);

	if (n->mNumMeshes > 0)
	{
		entity entity;

		// set trivial node variables
		entity.name = n->mName.C_Str();
		entity.type = type;
		entity.mesh_index = n->mMeshes[0];
		entity.model_bounds = compute_bounds_of_mesh(meshes_[n->mMeshes[0]]);

		// set translation, rotation and scale of this node
		glm::decompose(M, entity.TRS.scale, entity.TRS.rotation, entity.TRS.translate, glm::vec3(), glm::vec4());
		entity.TRS.rotation = glm::normalize(glm::conjugate(entity.TRS.rotation));
		entity.TRS.local = M;

		scene_.push_back(entity);
	}

	// travers child nodes
	for (size_t i = 0; i < n->mNumChildren; i++)
	{
		if (strcmp(n->mChildren[i]->mName.C_Str(),"Lights") != 0 && // skip lights
			(n->mChildren[i]->mNumChildren > 0 || n->mChildren[i]->mNumMeshes > 0)) // skip empty nodes
		{
		traverse_tree(n->mChildren[i], M, type);
		}
	}
}

glm::mat4 level::to_glm_mat4(const aiMatrix4x4& mat)
{
	glm::mat4 result;
	result[0][0] = static_cast<float>(mat.a1); result[0][1] = static_cast<float>(mat.b1);  result[0][2] = static_cast<float>(mat.c1); result[0][3] = static_cast<float>(mat.d1);
	result[1][0] = static_cast<float>(mat.a2); result[1][1] = static_cast<float>(mat.b2);  result[1][2] = static_cast<float>(mat.c2); result[1][3] = static_cast<float>(mat.d2);
	result[2][0] = static_cast<float>(mat.a3); result[2][1] = static_cast<float>(mat.b3);  result[2][2] = static_cast<float>(mat.c3); result[2][3] = static_cast<float>(mat.d3);
	result[3][0] = static_cast<float>(mat.a4); result[3][1] = static_cast<float>(mat.b4);  result[3][2] = static_cast<float>(mat.c4); result[3][3] = static_cast<float>(mat.d4);
	return result;
}

void level::setup_buffers()
{
	std::cout << "setup buffers..." << std::endl;

	const buffer vbo(0);
	vbo.reserve_memory(static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data());
	const buffer ebo(0);
	ebo.reserve_memory(static_cast<GLsizeiptr>(indices_.size() * sizeof(GLuint)), indices_.data());

	glCreateVertexArrays(1, &vao_);
	glVertexArrayElementBuffer(vao_, ebo.get_id());
	glVertexArrayVertexBuffer(vao_, 0, vbo.get_id(), 0, sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2));
	// position
	glEnableVertexArrayAttrib(vao_, 0);
	glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao_, 0, 0);
	// normal
	glEnableVertexArrayAttrib(vao_, 1);
	glVertexArrayAttribFormat(vao_, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
	glVertexArrayAttribBinding(vao_, 1, 0);
	// uv
	glEnableVertexArrayAttrib(vao_, 2);
	glVertexArrayAttribFormat(vao_, 2, 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec3) + sizeof(glm::vec3));
	glVertexArrayAttribBinding(vao_, 2, 0);

	ibo_.reserve_memory(static_cast<GLsizeiptr>(meshes_.size() * sizeof(draw_elements_indirect_command)), nullptr);
	matrix_ssbo_.reserve_memory(4, static_cast<GLsizeiptr>(meshes_.size() * sizeof(glm::mat4)), nullptr);
	tex_ssbo_.reserve_memory(5, static_cast<GLsizeiptr>(materials_.size() * sizeof(material)), materials_.data());
}

void level::load_lights(const aiScene* scene) {

	std::cout << "loading lights..." << std::endl;

	// collect light sources
	std::unordered_map<std::string, aiLight*> light_map;
	for (size_t i = 0; i < scene->mNumLights; i++)
	{
		aiLight* light = scene->mLights[i];
		light_map.insert({light->mName.C_Str(), light});
	}

	// find the light node
	aiNode* lights = nullptr;
	for (size_t i = 0; i < scene->mRootNode->mNumChildren; i++)
	{
		if (strcmp(scene->mRootNode->mChildren[i]->mName.C_Str(), "Lights") == 0)
		{
			lights = scene->mRootNode->mChildren[i];
			break;
		}
	}

	// extract light soruces
	for (size_t i = 0; i < lights->mNumChildren; i++)
	{
		aiNode* child = lights->mChildren[i];

		if (child->mNumChildren == 1) // directional light
		{
			aiNode* pre = child; // get prerotation
			glm::quat pre_rot = glm::quat_cast(to_glm_mat4(pre->mTransformation));

			aiNode* post = pre->mChildren[0]; // get prerotation
			glm::quat post_rot = glm::quat_cast(to_glm_mat4(post->mTransformation));

			aiNode* lig = post->mChildren[0]; // get light
			std::string name = lig->mName.C_Str();
			aiLight* light = light_map.at(name);
			assert(light->mType == aiLightSource_DIRECTIONAL);
			glm::quat lig_rot = glm::quat_cast(to_glm_mat4(lig->mTransformation));


			glm::quat finalRot = pre_rot * lig_rot * post_rot;

			const aiVector3D dir = light->mDirection;
			const aiColor3D col = light->mColorDiffuse;

			glm::vec3 direction = glm::vec3(dir.x, dir.y, dir.z);	
			direction = -glm::rotate(finalRot, direction);	// light direction gets inverted in shader
			glm::vec4 intensity = glm::vec4(col.r, col.g, col.b, 1.0f) * 1.0f; // increase light intensity (maya normalizes it)

			this->lights_.directional.push_back(directional_light{glm::vec4(direction,1.0f), intensity});
		}
		else // point light
		{
			std::string name = child->mName.C_Str();
			aiLight* light = light_map.at(name);
			assert(light->mType == aiLightSource_POINT);
			glm::mat4 M = to_glm_mat4(child->mTransformation);

			const aiColor3D col = light->mColorDiffuse;
			glm::vec4 position = glm::vec4(0,0,0,1.0f);

			position = M * position;
			glm::vec4 intensity = glm::vec4(col.r, col.g, col.b, 1.0f);

			this->lights_.point.push_back(positional_light{ position, intensity });
		}
	}
}

void level::load_shaders()
{
	aabb_viewer_ = std::make_unique<program>();
	Shader bounds_vert("../../assets/shaders/Testing/AABBviewer.vert");
	Shader bounds_frag("../../assets/shaders/Testing/AABBviewer.frag");
	aabb_viewer_->build_from(bounds_vert, bounds_frag);

	frustumviewer_ = std::make_unique<program>();
	Shader frustum_vert("../../assets/shaders/Testing/Frustumviewer.vert");
	frustumviewer_->build_from(frustum_vert, bounds_frag);
}

std::vector<physics_mesh> level::get_rigid()
{
	return rigid_;
}

std::vector<physics_mesh> level::get_dynamic()
{
	return dynamic_;
}

void level::collect_physic_meshes()
{
	rigid_.reserve(meshes_.size());
	dynamic_.reserve(meshes_.size());
	
	for (size_t i = 0; i < scene_.size(); i++)
	{
		entity& entity = scene_[i];

		if (entity.type == rigid || entity.type == dynamic)
		{
			glm::mat4 node_matrix = entity.get_node_matrix();
			uint32_t model_index = entity.mesh_index;
			uint32_t vtx_offset = meshes_[model_index].vertex_offset;
			uint32_t vtx_count = meshes_[model_index].vertex_count;
			physics_mesh phy_mesh;

			transformation trs;
			glm::decompose(node_matrix, trs.scale, trs.rotation, trs.translate, glm::vec3(), glm::vec4());
			trs.rotation = glm::normalize(glm::conjugate(trs.rotation));

			for (uint32_t j = 0; j != vtx_count; j++)
			{
				auto vertex_offset = (vtx_offset + j) * 8;
				const float* vf = &vertices[vertex_offset];

				phy_mesh.vtx_positions.push_back(vf[0]);
				phy_mesh.vtx_positions.push_back(vf[1]);
				phy_mesh.vtx_positions.push_back(vf[2]);
			}

			phy_mesh.model_trs = trs;
			phy_mesh.entity = &scene_[i];
			if (entity.type == rigid)
				rigid_.emplace_back(phy_mesh);
			else
				dynamic_.emplace_back(phy_mesh);
		}
	}
}

void level::draw_scene() {

	OPTICK_PUSH("update scene")
	// update view frustum
	if (!state_->freeze_cull)
	{
		OPTICK_PUSH("update frustum culler")
		frustum_culler::cull_view_proj = perframe_data_->view_proj;
		frustum_culler::get_frustum_planes(frustum_culler::cull_view_proj, frustum_culler::frustum_planes);
		frustum_culler::get_frustum_corners(frustum_culler::cull_view_proj, frustum_culler::frustum_corners);
		OPTICK_POP()
	}
	OPTICK_POP()

	OPTICK_PUSH("build render queue")
	update_render_queue(false);
	OPTICK_POP()

	// draw mesh
	OPTICK_PUSH("draw scene")
	ibo_.update(static_cast<GLsizeiptr>(queue_scene_.commands.size() * sizeof(draw_elements_indirect_command)), queue_scene_.commands.data());
	
	/// mode - draw triangles from every 3 indices
	/// type - data type of the indices vector
	/// indirect - offset into commands buffer, which is zero
	/// drawcount - is the number of draw calls that should be generated
	/// stride - because the commands are packed tightly aka just as descriped in the GL specs
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, static_cast<GLvoid*>(nullptr), static_cast<GLsizei>(queue_scene_.commands.size()), 0);
	
	OPTICK_POP()

#ifdef _DEBUG
	if (state_->cull_debug) // bounding box & frustum culling debug view
	{
		OPTICK_PUSH("draw debug AABB")
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_BLEND);
		aabb_viewer_->use();
		aabb_viewer_->set_vec4("lineColor", glm::vec4(0.0f,1.0f,0.0f, .1f));
			draw_aabbs(); // draw AABBs
		frustumviewer_->use();
		frustumviewer_->set_vec4("lineColor", glm::vec4(1.0f, 1.0f, 0.0f, .1f));
		frustumviewer_->set_vec3("corner0", frustum_culler::frustum_corners[0]);
		frustumviewer_->set_vec3("corner1", frustum_culler::frustum_corners[1]);
		frustumviewer_->set_vec3("corner2", frustum_culler::frustum_corners[2]);
		frustumviewer_->set_vec3("corner3", frustum_culler::frustum_corners[3]);
		frustumviewer_->set_vec3("corner4", frustum_culler::frustum_corners[4]);
		frustumviewer_->set_vec3("corner5", frustum_culler::frustum_corners[5]);
		frustumviewer_->set_vec3("corner6", frustum_culler::frustum_corners[6]);
		frustumviewer_->set_vec3("corner7", frustum_culler::frustum_corners[7]);
			glDrawArrays(GL_TRIANGLES, 0, 36); // draw frustum
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		// output frustum culling information for debugging every 2 seconds
		if (state_->cull)
		{
			frustum_culler::seconds_since_flush += perframe_data_->delta_time.x;
			if (frustum_culler::seconds_since_flush >= 2)
			{
				std::cout << "Models Loaded: " << frustum_culler::models_loaded << ", Models rendered: " << frustum_culler::models_visible
					<< ", Models culled: " << frustum_culler::models_loaded - frustum_culler::models_visible << "\n";
				frustum_culler::seconds_since_flush = 0;
			}
		}
		OPTICK_POP()
	}
#endif
}

void level::draw_scene_shadow_map()
{
	OPTICK_PUSH("update scene")

	// recalculate bounds & set lod uniforms
	if (perframe_data_->delta_time.y > 60.0f)
	{
		glm::vec3 t = scene_[lava_].TRS.translate;
		t.y += perframe_data_->delta_time.x * .2f;
		scene_[lava_].set_node_trs(t, scene_[lava_].TRS.rotation, scene_[lava_].TRS.scale);
		state_->lava_height = scene_[lava_].TRS.translate.y;
	}
	OPTICK_PUSH("transform bounding boxes")
	OPTICK_POP()
	OPTICK_PUSH("update frustum culler uniform")
	lod_system::near_plane = perframe_data_->ssao1.z;
	lod_system::view_pos = perframe_data_->view_pos;
	glm::mat4 vp = glm::transpose(perframe_data_->view_proj);
	lod_system::view_dir = vp[3];
	frustum_culler::models_visible = 0;
	OPTICK_POP()
		
	OPTICK_PUSH("build render queue")
	update_render_queue(true);
	OPTICK_POP()
	OPTICK_POP()

	// draw mesh
	OPTICK_PUSH("draw scene")
	glBindVertexArray(vao_);

	//glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	matrix_ssbo_.update(static_cast<GLsizeiptr>(sizeof(glm::mat4) * queue_scene_.model_matrices.size()), queue_scene_.model_matrices.data());
	ibo_.update(static_cast<GLsizeiptr>(queue_scene_.commands.size() * sizeof(draw_elements_indirect_command)), queue_scene_.commands.data());
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, static_cast<GLvoid*>(nullptr), static_cast<GLsizei>(queue_scene_.commands.size()), 0);
	
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	OPTICK_POP()
}

void level::build_render_queue() {
	for (const entity& entity : scene_)
	{

		uint32_t instanceCount = 1;
		if (!entity.game_properties.is_active)
			instanceCount = 0;

		const glm::mat4 node_matrix = entity.get_node_matrix();
		const uint32_t mesh_index = entity.mesh_index;
		const uint32_t material_index = meshes_[mesh_index].material_index;
		const uint32_t model_index = queue_scene_.model_matrices.size();
		if (materials_[material_index].type == invisible)
			instanceCount = 0;
		uint32_t LOD = 0;
		
		const uint32_t count = meshes_[mesh_index].index_count[LOD];
		const uint32_t firstIndex = meshes_[mesh_index].index_offset[LOD];
		const uint32_t baseVertex = meshes_[mesh_index].vertex_offset;
		const uint32_t baseInstance = material_index + (static_cast<uint32_t>(model_index) << 16);

		draw_elements_indirect_command cmd = draw_elements_indirect_command{
			count,
			instanceCount,
			firstIndex,
			baseVertex,
			baseInstance };

		queue_scene_.commands.push_back(cmd);
		queue_scene_.model_matrices.push_back(node_matrix);
		
		frustum_culler::models_visible += cmd.instanceCount_;
	}
}

void level::update_render_queue(const bool for_shadow) {
	for (size_t i = 0; i < queue_scene_.commands.size(); i++)
	{
		entity& entity = scene_[i];
		draw_elements_indirect_command& cmd = queue_scene_.commands[i];

		if (for_shadow)
		{
			cmd.instanceCount_ = 1;
			if (!entity.game_properties.is_active)
				cmd.instanceCount_ = 0;
			const glm::mat4 node_matrix = entity.get_node_matrix();
			const uint32_t mesh_index = entity.mesh_index;
			const uint32_t material_index = meshes_[mesh_index].material_index;
			if (materials_[material_index].type == invisible)
				cmd.instanceCount_ = 0;
			cmd.baseInstance_ = material_index + (i << 16);
			queue_scene_.model_matrices[i] = node_matrix;
		}else
		{
			if (state_->cull && cmd.instanceCount_ == 1)
			{
				if (!frustum_culler::is_box_in_frustum(frustum_culler::frustum_planes, frustum_culler::frustum_corners, entity.world_bounds))
					cmd.instanceCount_ = 0;
			}

			const uint32_t mesh_index = entity.mesh_index;
			uint32_t LOD = lod_system::decide_lod(meshes_[mesh_index].index_count.size(), entity.world_bounds);
			cmd.count_ = meshes_[mesh_index].index_count[LOD];
			cmd.firstIndex_ = meshes_[mesh_index].index_offset[LOD];

			frustum_culler::models_visible += cmd.instanceCount_;
		}
	}
}

void level::draw_aabbs() const
{
	for (const entity& entity : scene_)
	{
		bounding_box bounds = entity.world_bounds;
		aabb_viewer_->set_vec3("min", entity.world_bounds.min_);
		aabb_viewer_->set_vec3("max", entity.world_bounds.max_);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

glm::mat4 level::get_tight_scene_frustum(glm::mat4 light_view) const
{
	glm::vec3 min = scene_bounds_.min_;
	glm::vec3 max = scene_bounds_.max_;

	glm::vec3 corners[] = {
			glm::vec3(min.x, min.y, min.z),
			glm::vec3(min.x, max.y, min.z),
			glm::vec3(min.x, min.y, max.z),
			glm::vec3(min.x, max.y, max.z),
			glm::vec3(max.x, min.y, min.z),
			glm::vec3(max.x, max.y, min.z),
			glm::vec3(max.x, min.y, max.z),
			glm::vec3(max.x, max.y, max.z),
	};
	for (auto& v : corners)
		v = glm::vec3(light_view * glm::vec4(v, 1.0f));

	glm::vec3 vmin(std::numeric_limits<float>::max());
	glm::vec3 vmax(std::numeric_limits<float>::lowest());

	for (auto& corner : corners)
	{
		vmin = glm::min(vmin, corner);
		vmax = glm::max(vmax, corner);
	}
	min = vmin;
	max = vmax;

	return glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);
}

void level::release() const
{
	glDeleteVertexArrays(1, &vao_);

	for (auto material : materials_)
	{
		material::clear(material);
	}
}
