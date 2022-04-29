#include "Level.h"
#include "Program.h"
#include <meshoptimizer/meshoptimizer.h>
#include <unordered_map>
#include <thread>

/// @brief loads an fbx file from the given path and converts it to useable data structures
/// @param scenePath location of the fbx file, expected to be in "assets"
Level::Level(const char* scenePath, std::shared_ptr<GlobalState> state, PerFrameData& pfdata) {

	// 1. load fbx file into assimps internal data structures and apply various preprocessing to the data
	std::cout << "import scene from fbx file..." << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(scenePath,
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_FlipUVs |
		aiProcess_FixInfacingNormals |
		aiProcess_ValidateDataStructure | 
		0);

	if (!&scene){
		std::cerr << "ERROR: Couldn't load scene" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::thread mesh(&Level::loadMeshes, this, std::ref(scene));

	std::thread light(&Level::loadLights, this, std::ref(scene));


	loadMaterials(scene);

	// build scene graph and calculate AABBs
	std::cout << "build scene hierarchy..." << std::endl;
	mesh.join();
	traverseTree(scene->mRootNode, nullptr, &sceneGraph);
	transformBoundingBoxes(&sceneGraph, glm::mat4(1));

	setupVertexBuffers();
	setupDrawBuffers();

	// finalize
	light.join();
	loadShaders();
	this->state = state;
	perframeData = &pfdata;

	std::cout << std::endl;
}

void Level::loadMeshes(const aiScene* scene)
{
	globalVertexOffset = 0;
	globalIndexOffset = 0;

	meshes.reserve(scene->mNumMeshes);

	std::cout << "loading meshes..." << std::endl;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(extractMesh2(mesh));
	}
	ModelsLoaded = meshes.size();
}

/// @brief extracts position, normal and uvs with the correlating indices from an assimp mesh
/// @param mesh is a single meshm with a unique material
/// @return a mesh but in usable structures for drawing it
subMesh Level::extractMesh(const aiMesh* mesh)
{
	subMesh m;
	m.name = mesh->mName.C_Str();
	m.indexOffset = globalIndexOffset;
	m.vertexOffset = globalVertexOffset;
	m.vertexCount = mesh->mNumVertices;
	m.materialIndex = mesh->mMaterialIndex;

	// extract vertices from the aimesh
	for (size_t j = 0; j < mesh->mNumVertices; j++)
	{
		const aiVector3D p = mesh->HasPositions() ? mesh->mVertices[j] : aiVector3D(0.0f);
		const aiVector3D n = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f, 1.0f, 0.0f);
		const aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.5f, 0.5f, 0.0f);

		vertices.push_back(p.x);
		vertices.push_back(p.y);
		vertices.push_back(p.z);

		vertices.push_back(n.x);
		vertices.push_back(n.y);
		vertices.push_back(n.z);

		vertices.push_back(t.x);
		vertices.push_back(t.y);
	}

	uint32_t indexCount = 0;
	//extract indices from the aimesh
	for (size_t j = 0; j < mesh->mNumFaces; j++)
	{
		for (unsigned k = 0; k != mesh->mFaces[j].mNumIndices; k++)
		{
			GLuint index = mesh->mFaces[j].mIndices[k] + globalIndexOffset;
			indexCount++;
			indices.push_back(index);
		}
	}

	m.indexCount = indexCount;

	globalVertexOffset += mesh->mNumVertices;
	globalIndexOffset += indexCount;

	printf("Mesh [%s] %u\n", mesh->mName.C_Str(), meshes.size() + 1);
	return m;
}

/// @brief extracts position, normal and uvs with the correlating indices from an assimp mesh
/// @param mesh is a single meshm with a unique material
/// @return a mesh but in usable structures for drawing it
subMesh Level::extractMesh2(const aiMesh* mesh)
{
	struct Vertex
	{
		float px, py, pz;
		float nx, ny, nz;
		float tx, ty;
	};

	printf("Mesh [%s] %u\n", mesh->mName.C_Str(), meshes.size() + 1);
	subMesh m;
	m.name = mesh->mName.C_Str();
	m.indexOffset = globalIndexOffset;
	m.vertexOffset = globalVertexOffset;
	m.materialIndex = mesh->mMaterialIndex;
			
	std::vector<Vertex> rawVertices;
	std::vector <unsigned int> rawIndices;			

	// extract vertices from the aimesh
	for (size_t j = 0; j < mesh->mNumVertices; j++)
	{
		const aiVector3D p = mesh->HasPositions() ? mesh->mVertices[j] : aiVector3D(0.0f);
		const aiVector3D n = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f, 1.0f, 0.0f);
		const aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.5f, 0.5f, 0.0f);

		Vertex vtx =
		{
			p.x, p.y, p.z,
			n.x, n.y, n.z,
			t.x, t.y
		};

		rawVertices.push_back(vtx);
	}

	//extract indices from the aimesh
	for (size_t j = 0; j < mesh->mNumFaces; j++)
	{
		for (unsigned k = 0; k != mesh->mFaces[j].mNumIndices; k++)
		{
			GLuint index = mesh->mFaces[j].mIndices[k];
			rawIndices.push_back(index);
		}
	}

	const auto vtxStride = sizeof(Vertex);

	// re-index geometry
	std::vector<unsigned int> remap(rawIndices.size());
	size_t vertex_count = meshopt_generateVertexRemap(remap.data(), rawIndices.data(), rawIndices.size(), rawVertices.data(), rawIndices.size(), vtxStride);

	std::vector <unsigned int> opt_indices(rawIndices.size());
	std::vector<Vertex> opt_vertices(vertex_count);

	meshopt_remapIndexBuffer(opt_indices.data(), rawIndices.data(), rawIndices.size(), remap.data());
	meshopt_remapVertexBuffer(opt_vertices.data(), rawVertices.data(), rawVertices.size(), vtxStride, remap.data());

	// further optimize geometry
	//meshopt_optimizeVertexCache(opt_indices.data(), opt_indices.data(), rawIndices.size(), vertex_count);
	//meshopt_optimizeOverdraw(opt_indices.data(), opt_indices.data(), rawIndices.size(), &opt_vertices[0].px, vertex_count, vtxStride, 1.05f);
	//meshopt_optimizeVertexFetch(opt_vertices.data(), opt_indices.data(), rawIndices.size(), opt_vertices.data(), vertex_count, vtxStride);
	
	m.vertexCount = opt_vertices.size();
	m.indexCount = opt_indices.size();

	std::vector<float> resultVertices;
	for (const auto& vertex : opt_vertices)
	{
		resultVertices.push_back(vertex.px);
		resultVertices.push_back(vertex.py);
		resultVertices.push_back(vertex.pz);
		resultVertices.push_back(vertex.nx);
		resultVertices.push_back(vertex.ny);
		resultVertices.push_back(vertex.nz);
		resultVertices.push_back(vertex.tx);
		resultVertices.push_back(vertex.ty);
	}

	globalVertexOffset += m.vertexCount;
	globalIndexOffset += m.indexCount;

	vertices.insert(vertices.end(), resultVertices.begin(), resultVertices.end());
	indices.insert(indices.end(), opt_indices.begin(), opt_indices.end());

	return m;
}

/// @brief finds the maximum and minimum vertex positions of all meshes, which should define the bounds
BoundingBox Level::computeBoundsOfMesh(subMesh mesh) {
	const auto numIndices = mesh.vertexCount;

	glm::vec3 vmin(std::numeric_limits<float>::max());
	glm::vec3 vmax(std::numeric_limits<float>::lowest());

	for (auto i = 0; i != numIndices; i++)
	{
		auto vertexOffset = (mesh.vertexOffset + i) * 8;
		const float* vf = &vertices[vertexOffset];

		vmin = glm::min(vmin, glm::vec3(vf[0], vf[1], vf[2]));
		vmax = glm::max(vmax, glm::vec3(vf[0], vf[1], vf[2]));
	}
	return BoundingBox(vmin, vmax);
}

void Level::transformBoundingBoxes(Hierarchy* node, glm::mat4 globalTransform)
{
	boolean isLeaf = node->modelIndices.size() > 0;
	glm::mat4 M = globalTransform * node->getNodeMatrix();

	if (isLeaf) // transform model bounds to world coordinates
	{
		BoundingBox bounds = node->modelBounds;
		glm::mat4 M = globalTransform * node->getNodeMatrix();
		bounds.min_ = M * glm::vec4(bounds.min_, 1.0f);
		bounds.max_ = M * glm::vec4(bounds.max_, 1.0f);
		node->nodeBounds = BoundingBox(bounds.min_, bounds.max_);
	}

	// tranform all child nodes bounds
	for (size_t i = 0; i < node->children.size(); i++)
	{
		transformBoundingBoxes(&node->children[i], M);
	}
	
	if (!isLeaf) // calculate the node bound from childrens bounds
	{
		glm::vec3 vmin(std::numeric_limits<float>::max());
		glm::vec3 vmax(std::numeric_limits<float>::lowest());

		for (size_t i = 0; i < node->children.size(); i++)
		{
			glm::vec3 cmin = node->children[i].nodeBounds.min_;
			glm::vec3 cmax = node->children[i].nodeBounds.max_;

			vmin = glm::min(vmin, cmin);
			vmax = glm::max(vmax, cmax);
		}

		BoundingBox bounds = BoundingBox(vmin, vmax);
		glm::mat4 M = globalTransform * node->getNodeMatrix();
		bounds.min_ = M * glm::vec4(bounds.min_, 1.0f);
		bounds.max_ = M * glm::vec4(bounds.max_, 1.0f);
		node->nodeBounds = BoundingBox(bounds.min_, bounds.max_);
	}
}

/// @brief loads all materials (textures) from the material assimp provides
/// @param M is a single material and should contain 5 aiTextureTypes, only one of them is needed for loading the textures
/// @return a material object containing opengl handles to the fives loaded textures
void Level::loadMaterials(const aiScene* scene)
{
	std::cout << "loading materials..." << std::endl;

	for (size_t m = 0; m < scene->mNumMaterials; m++)
	{
		aiMaterial* mm = scene->mMaterials[m];

		printf("Material [%s] %u\n", mm->GetName().C_Str(), m + 1);

		aiString Path;

		if (aiGetMaterialTexture(mm, aiTextureType_BASE_COLOR, 0, &Path) == AI_SUCCESS)
		{
			const std::string albedoMap = std::string(Path.C_Str());
		}

		//	all other materials can be found with: aiTextureType_NORMAL_CAMERA/_METALNESS/_DIFFUSE_ROUGHNESS/_AMBIENT_OCCLUSION

		Material mat = Material(Path.C_Str(), mm->GetName().C_Str());
		materials.push_back(mat);
		RenderItem item;
		item.material = mm->GetName().C_Str();
		renderQueue.push_back(item);
	}
}

/// @brief recursive function that builds a scenegraph with hierarchical transformation, similiar to assimps scene
/// @param n is an assimp node that holds transformations, nodes or meshes
/// @param parent is the parent node of the currently created node, mainly used for debugging
/// @param node is the current node from the view of the parent node
void Level::traverseTree(aiNode* n, Hierarchy* parent, Hierarchy* node)
{
	// set trivial node variables
	const glm::mat4 M = toGlmMat4(n->mTransformation);
	node->name = n->mName.C_Str();
	node->parent = parent;

	// add a all mesh indices to this node (assumes only 1 mesh per node) and calulate bounds in model space
	for (unsigned int i = 0; i < n->mNumMeshes; i++)
	{
		node->modelIndices.push_back(n->mMeshes[i]);
		node->modelBounds = computeBoundsOfMesh(meshes[n->mMeshes[i]]);
	}

	// set translation, rotation and scale of this node
	glm::decompose(M, node->TRS.Scale, node->TRS.Rotation, node->TRS.Translate, glm::vec3(), glm::vec4());
	node->TRS.Rotation = glm::normalize(glm::conjugate(node->TRS.Rotation));

	// travers child nodes
	for (size_t i = 0; i < n->mNumChildren; i++)
	{
		if (strcmp(n->mChildren[i]->mName.C_Str(),"Lights") != 0 && // skip lights
			(n->mChildren[i]->mNumChildren > 0 || n->mChildren[i]->mNumMeshes > 0)) // skip empty nodes
		{
		Hierarchy child;
		traverseTree(n->mChildren[i], node, &child);
		node->children.push_back(child);
		}
	}
}


/// @brief simple helper function for converting the assimp 4x4 matrices to 4x4 glm matrices
/// @param mat is a 4x4 matrix from assimp
/// @return a 4x4 matrix in glm format
glm::mat4 Level::toGlmMat4(const aiMatrix4x4& mat)
{
	glm::mat4 result;
	result[0][0] = (float)mat.a1; result[0][1] = (float)mat.b1;  result[0][2] = (float)mat.c1; result[0][3] = (float)mat.d1;
	result[1][0] = (float)mat.a2; result[1][1] = (float)mat.b2;  result[1][2] = (float)mat.c2; result[1][3] = (float)mat.d2;
	result[2][0] = (float)mat.a3; result[2][1] = (float)mat.b3;  result[2][2] = (float)mat.c3; result[2][3] = (float)mat.d3;
	result[3][0] = (float)mat.a4; result[3][1] = (float)mat.b4;  result[3][2] = (float)mat.c4; result[3][3] = (float)mat.d4;
	return result;
}

/// @brief Creates and fills vertex and index buffers and sets up the "big" vao which will suffice to render all meshes
void Level::setupVertexBuffers()
{
	std::cout << "setup buffers..." << std::endl;

	glCreateBuffers(1, &VBO);
	glNamedBufferStorage(VBO, vertices.size() * sizeof(float), vertices.data(), 0);
	glCreateBuffers(1, &EBO);
	glNamedBufferStorage(EBO, indices.size() * sizeof(GLuint), indices.data(), 0);

	glCreateVertexArrays(1, &VAO);
	glVertexArrayElementBuffer(VAO, EBO);
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2));
	// position
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	// normal
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
	glVertexArrayAttribBinding(VAO, 1, 0);
	// uv
	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_TRUE, sizeof(glm::vec3) + sizeof(glm::vec3));
	glVertexArrayAttribBinding(VAO, 2, 0);
}

/// @brief sets up indirect command and shader storage buffers for efficient und reduced render calls
void Level::setupDrawBuffers()
{

	glCreateBuffers(1, &IBO);
	glNamedBufferStorage(IBO, meshes.size() * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &matrixSSBO);
	glNamedBufferStorage(matrixSSBO, meshes.size() * sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, matrixSSBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
}

/// @brief loads all directional and positional lights in the assimp scene, corrects position for positional lights by traversing the tree
/// @param scene is the scene containing the lights and root node
void Level::loadLights(const aiScene* scene) {

	std::cout << "loading lights..." << std::endl;

	// collect light sources
	std::unordered_map<std::string, aiLight*> lightMap;
	for (size_t i = 0; i < scene->mNumLights; i++)
	{
		aiLight* light = scene->mLights[i];
		lightMap.insert({light->mName.C_Str(), light});
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
			glm::quat preRot = glm::quat_cast(toGlmMat4(pre->mTransformation));

			aiNode* post = pre->mChildren[0]; // get prerotation
			glm::quat postRot = glm::quat_cast(toGlmMat4(post->mTransformation));

			aiNode* lig = post->mChildren[0]; // get light
			std::string name = lig->mName.C_Str();
			aiLight* light = lightMap.at(name);
			assert(light->mType == aiLightSource_DIRECTIONAL);
			glm::quat ligRot = glm::quat_cast(toGlmMat4(lig->mTransformation));


			glm::quat finalRot = preRot * ligRot * postRot;

			const aiVector3D dir = light->mDirection;
			const aiColor3D col = light->mColorDiffuse;

			glm::vec3 direction = glm::vec3(dir.x, dir.y, dir.z);	
			direction = -glm::rotate(finalRot, direction);	// light direction gets inverted in shader
			direction = glm::vec3(0.0001, 1, 0); // todo
			glm::vec4 intensity = glm::vec4(col.r, col.g, col.b, 1.0f) * 3.0f; // double the light intensity (maya normalizes it)

			this->lights.directional.push_back(DirectionalLight{glm::vec4(direction,1.0f), intensity});
		}
		else // point light
		{
			std::string name = child->mName.C_Str();
			aiLight* light = lightMap.at(name);
			assert(light->mType == aiLightSource_POINT);
			glm::mat4 M = toGlmMat4(child->mTransformation);

			const aiColor3D col = light->mColorDiffuse;
			glm::vec4 position = glm::vec4(0,0,0,1.0f);

			position = M * position;
			glm::vec4 intensity = glm::vec4(col.r, col.g, col.b, 1.0f);

			this->lights.point.push_back(PositionalLight{ position, intensity });
		}
	}
}

void Level::loadShaders()
{
	AABBviewer = std::unique_ptr<Program>(new Program);
	Shader boundsVert("../../assets/shaders/Testing/AABBviewer.vert");
	Shader boundsFrag("../../assets/shaders/Testing/AABBviewer.frag");
	AABBviewer->buildFrom(boundsVert, boundsFrag);

	Frustumviewer = std::unique_ptr<Program>(new Program);
	Shader FrustumVert("../../assets/shaders/Testing/Frustumviewer.vert");
	Frustumviewer->buildFrom(FrustumVert, boundsFrag);
}

std::vector<PhysicsMesh> Level::getRigid()
{
	Hierarchy* rigidNode = nullptr;
	for (size_t i = 0; i < sceneGraph.children.size(); i++)
	{
		if (sceneGraph.children[i].name.compare("Rigid") == 0) {
			rigidNode = &sceneGraph.children[i];
		}
	}
	collectRigidPhysicMeshes(rigidNode, glm::mat4(1));
	return rigid;
}

std::vector<PhysicsMesh> Level::getDynamic()
{
	dynamicNode = nullptr;
	for (size_t i = 0; i < sceneGraph.children.size(); i++)
	{
		if (sceneGraph.children[i].name.compare("Dynamic") == 0) {
			dynamicNode = &sceneGraph.children[i];
		}
	}
	collectDynamicPhysicMeshes(dynamicNode, glm::mat4(1));
	return dynamic;
}

void Level::collectRigidPhysicMeshes(Hierarchy* node, glm::mat4 globalTransform)
{
	glm::mat4 nodeMatrix = globalTransform * node->getNodeMatrix();

	for (uint32_t i = 0; i < node->modelIndices.size(); i++)
	{
		uint32_t modelindex = node->modelIndices[i];
		uint32_t vtxOffset = meshes[modelindex].vertexOffset;
		uint32_t vtxCount = meshes[modelindex].vertexCount;
		PhysicsMesh phyMesh;

		Transformation trs;
		glm::decompose(nodeMatrix, trs.Scale, trs.Rotation, trs.Translate, glm::vec3(), glm::vec4());
		trs.Rotation = glm::normalize(glm::conjugate(trs.Rotation));

		for (auto i = 0; i != vtxCount; i++)
		{
			auto vertexOffset = (vtxOffset + i) * 8;
			const float* vf = &vertices[vertexOffset];

			phyMesh.vtxPositions.push_back(vf[0]);
			phyMesh.vtxPositions.push_back(vf[1]);
			phyMesh.vtxPositions.push_back(vf[2]);
		}

		phyMesh.modelTRS = trs;
		phyMesh.node = node;
		rigid.push_back(phyMesh);
	}

	for (size_t i = 0; i < node->children.size(); i++)
	{
		collectRigidPhysicMeshes(&node->children[i], nodeMatrix);
	}
}

void Level::collectDynamicPhysicMeshes(Hierarchy* node, glm::mat4 globalTransform)
{
	glm::mat4 nodeMatrix = globalTransform * node->getNodeMatrix();

	for (uint32_t i = 0; i < node->modelIndices.size(); i++)
	{
		uint32_t modelindex = node->modelIndices[i];
		uint32_t vtxOffset = meshes[modelindex].vertexOffset;
		uint32_t vtxCount = meshes[modelindex].vertexCount;
		PhysicsMesh phyMesh;

		Transformation trs;
		glm::decompose(nodeMatrix, trs.Scale, trs.Rotation, trs.Translate, glm::vec3(), glm::vec4());
		trs.Rotation = glm::normalize(glm::conjugate(trs.Rotation));

		for (auto i = 0; i != vtxCount; i++)
		{
			auto vertexOffset = (vtxOffset + i) * 8;
			const float* vf = &vertices[vertexOffset];

			phyMesh.vtxPositions.push_back(vf[0]);
			phyMesh.vtxPositions.push_back(vf[1]);
			phyMesh.vtxPositions.push_back(vf[2]);
		}

		phyMesh.modelTRS = trs;
		phyMesh.node = node;
		dynamic.push_back(phyMesh);
	}

	for (size_t i = 0; i < node->children.size(); i++)
	{
		collectDynamicPhysicMeshes(&node->children[i], nodeMatrix);
	}
}

/// @brief sets up indirect render calls, binds the data and calls the actual draw routine
void Level::DrawScene() {

	// update view frustum
	if (!state->freezeCull_)
	{
		cullViewProj = perframeData->ViewProj;
		FrustumCulling::getFrustumPlanes(cullViewProj, frustumPlanes);
		FrustumCulling::getFrustumCorners(cullViewProj, frustumCorners);
	}

	transformBoundingBoxes(dynamicNode, glm::mat4(1));
	// flaten tree
	resetQueue();
	buildRenderQueue(&sceneGraph, glm::mat4(1));

	// draw mesh
	glBindVertexArray(VAO);

	for (size_t i = 0; i < renderQueue.size(); i++)
	{
		glNamedBufferSubData(matrixSSBO, 0, sizeof(glm::mat4) * renderQueue[i].modelMatrices.size(), renderQueue[i].modelMatrices.data());

		glNamedBufferSubData(IBO, 0, renderQueue[i].commands.size() * sizeof(DrawElementsIndirectCommand), renderQueue[i].commands.data());
		
		const GLuint textures[] = {materials[i].getAlbedo(), materials[i].getNormalmap(), materials[i].getMetallic(), materials[i].getRoughness(), materials[i].getAOmap() };

		glBindTextures(0, 5, textures);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, (GLsizei)renderQueue[i].commands.size(), 0);
		/// explaination:
		/// GL_TRIANGLES - draw triangles from every 3 indices
		/// GL_UNSIGNED_INT - data type of the indices vector
		/// (GLvoid*)0 - offset into commands buffer, which is zero
		/// (GLsizei)renderQueue[i].commands.size() - is the number of draw calls that should be generated
		/// 0 - because the commands are packed tightly aka just as descriped in the GL specs
	}

	if (state->cullDebug_) // bounding box & frustum culling debug view
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_BLEND);
		AABBviewer->Use();
		AABBviewer->setVec4("lineColor", glm::vec4(0.0f,1.0f,0.0f, .1f));
			DrawAABBs(sceneGraph); // draw AABBs
		Frustumviewer->Use();
		Frustumviewer->setVec4("lineColor", glm::vec4(1.0f, 1.0f, 0.0f, .1f));
		Frustumviewer->setVec3("corner0", frustumCorners[0]);
		Frustumviewer->setVec3("corner1", frustumCorners[1]);
		Frustumviewer->setVec3("corner2", frustumCorners[2]);
		Frustumviewer->setVec3("corner3", frustumCorners[3]);
		Frustumviewer->setVec3("corner4", frustumCorners[4]);
		Frustumviewer->setVec3("corner5", frustumCorners[5]);
		Frustumviewer->setVec3("corner6", frustumCorners[6]);
		Frustumviewer->setVec3("corner7", frustumCorners[7]);
			glDrawArrays(GL_TRIANGLES, 0, 36); // draw frustum
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		// output frustum culling information for debugging every 2 seconds
		if (state->cull_)
		{
			secondsSinceFlush += perframeData->deltaTime.x;
			if (secondsSinceFlush >= 2)
			{
				std::cout << "Models Loaded: " << ModelsLoaded << ", Models rendered: " << ModelsVisible
					<< ", Models culled: " << ModelsLoaded - ModelsVisible << "\n";
				secondsSinceFlush = 0;
			}
		}
	}

}

void Level::DrawSceneFromLightSource()
{
	boolean cull = state->cull_;
	state->cull_ = false;

	// flaten tree
	resetQueue();
	buildRenderQueue(&sceneGraph, glm::mat4(1));

	// draw mesh
	glBindVertexArray(VAO);

	for (size_t i = 0; i < renderQueue.size(); i++)
	{
		glNamedBufferSubData(matrixSSBO, 0, sizeof(glm::mat4) * renderQueue[i].modelMatrices.size(), renderQueue[i].modelMatrices.data());

		glNamedBufferSubData(IBO, 0, renderQueue[i].commands.size() * sizeof(DrawElementsIndirectCommand), renderQueue[i].commands.data());

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, (GLsizei)renderQueue[i].commands.size(), 0);
	}
	state->cull_ = cull;
}

/// @brief recursiveley travels the tree and adds any models it finds, according to its material, to the render queue
/// @param node that gets checked for models
/// @param globalTransform the summed tranformation matrices of all parent nodes
void Level::buildRenderQueue(const Hierarchy* node, glm::mat4 globalTransform) {
	if (!node->gameProperties.isActive)
		return;

	if (state->cull_)
	{
		if (!FrustumCulling::isBoxInFrustum(frustumPlanes, frustumCorners, node->nodeBounds))
			return;
	}

	glm::mat4 nodeMatrix = globalTransform * node->getNodeMatrix();
	for (size_t i = 0; i < node->modelIndices.size(); i++)
	{
		uint32_t meshIndex = node->modelIndices[i];
		uint32_t materialIndex = meshes[meshIndex].materialIndex;

		uint32_t count = meshes[meshIndex].indexCount;	// number of indices that get drawn, eg for single quad = 6
		uint32_t instanceCount = 1;	// number of instanced that get drawn, 0 means none, this programm doesn't use instanced rendering
		uint32_t firstIndex = meshes[meshIndex].indexOffset; // index offset, eg for first mesh = 0, sec mesh = firstIndexOffs + 0, etc
		uint32_t baseVertex = meshes[meshIndex].vertexOffset; // offset added before chosing vertices
		uint32_t baseInstance = renderQueue[materialIndex].modelMatrices.size(); // model matrix id, could be used for bindless textures

		DrawElementsIndirectCommand cmd= DrawElementsIndirectCommand{
			count,
			instanceCount,
			firstIndex,
			baseVertex,
			baseInstance };

		renderQueue[materialIndex].commands.push_back(cmd);
		renderQueue[materialIndex].modelMatrices.push_back(nodeMatrix);
		ModelsVisible++;
	}
	

	for (size_t i = 0; i < node->children.size(); i++)
	{
		buildRenderQueue(&node->children[i], nodeMatrix);
	}
}

/// @brief removes all render commands and model matrices of the render queue, should be done after each draw call
void Level::resetQueue()
{
	for (size_t i = 0; i < renderQueue.size(); i++)
	{
		renderQueue[i].commands.clear();
		renderQueue[i].modelMatrices.clear();
	}

	ModelsVisible = 0;
}

void Level::DrawAABBs(Hierarchy node)
{
	BoundingBox bounds = node.nodeBounds;
	AABBviewer->setVec3("min", node.nodeBounds.min_);
	AABBviewer->setVec3("max", node.nodeBounds.max_);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (size_t i = 0; i < node.children.size(); i++)
	{
		DrawAABBs(node.children[i]);
	}
}

glm::mat4 Level::getTightSceneFrustum()
{
	// rotate scene bounds from opengl view direction to camera direction
	glm::vec3 Ldir = lights.directional[0].direction;
	glm::vec3 Vdir = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::quat r = glm::rotation(Vdir, Ldir);
	glm::vec3 min = glm::rotate(r, sceneGraph.nodeBounds.min_);
	glm::vec3 max = glm::rotate(r, sceneGraph.nodeBounds.max_);
	//glm::vec3 min = sceneGraph.nodeBounds.min_;
	//glm::vec3 max = sceneGraph.nodeBounds.max_;

	return glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z); 
}

/// @brief cleans up all buffers and textures
void Level::Release()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &matrixSSBO);

	for (size_t i = 0; i < materials.size(); i++)
	{
		materials[i].clear();
	}
}