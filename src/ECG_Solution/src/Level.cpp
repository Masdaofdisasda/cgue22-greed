#include "Level.h"

Level::Level(const char* scenePath) {

	// 1. load fbx file into assimps internal data structures and apply various preprocessing to the data
	std::cout << "load scene... (this could take a while)" << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(scenePath,
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_FlipUVs |
		aiProcess_FixInfacingNormals |
		aiProcess_ValidateDataStructure
	);

	if (!scene)
	{
		std::cerr << "ERROR: Couldn't load scene" << std::endl;
	}

	globalVertexOffset = 0;
	globalIndexOffset = 0;

	meshes.reserve(scene->mNumMeshes);
	boxes.reserve(scene->mNumMeshes);

	// 2. iterate through the scene and create a mesh object for every aimesh in the scene
	std::cout << "loading meshes..." << std::endl;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(extractMesh(mesh));

	}

	// 3. load materials
	std::cout << "loading materials..." << std::endl;
	for (size_t m = 0; m < scene->mNumMaterials; m++)
	{
		aiMaterial* mm = scene->mMaterials[m];

		printf("Material [%s] %u\n", mm->GetName().C_Str(), m+1);

		Material mat = loadMaterials(mm);
		materials.push_back(mat);
	}


	// 4. create a model for drawing for every loaded mesh
	for (size_t i = 0; i < meshes.size(); i++)
	{
		Model draw;
		draw.meshIndex = (uint32_t)i;
		draw.materialIndex = meshes[i].materialIndex;
		draw.transformIndex = 0;
		models.push_back(draw);

	}

	// 5. build scene graph
	std::cout << "build scene hierarchy..." << std::endl;
	aiNode* n = scene->mRootNode;
	Hierarchy child;
	sceneGraph.parent = nullptr;
	sceneGraph.children.push_back(child);
	sceneGraph.localTransform = glm::mat4(1);
	traverseTree(n, &sceneGraph, &sceneGraph.children[0]);

	// 6. setup buffers for vertex and indices data
	std::cout << "setup buffers..." << std::endl;
	setupVertexBuffers();

	// 7. setup buffers for transform and drawcommands
	setupDrawBuffers(); //TODO

	// 8. load lights sources 
	std::cout << "loading lights..." << std::endl;
	loadLights(scene); //TODO

	std::cout << std::endl;
}

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
		const aiVector3D p = mesh->HasPositions() ? mesh->mVertices[j] : aiVector3D();
		const aiVector3D n = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D();
		const aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D();

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

void Level::calculateBoundingBoxes() {
	boxes.clear();

	for (const auto& mesh : meshes)
	{
		const auto numIndices = mesh.indexCount;

		glm::vec3 vmin(std::numeric_limits<float>::max());
		glm::vec3 vmax(std::numeric_limits<float>::lowest());

		for (auto i = 0; i != numIndices; i++)
		{
			auto vtxOffset = indices[mesh.indexOffset + i] + mesh.vertexOffset;
			const float* vf = &vertices[vtxOffset];
			vmin = glm::min(vmin, glm::vec3(vf[0], vf[1], vf[2]));
			vmax = glm::max(vmax, glm::vec3(vf[0], vf[1], vf[2]));
		}

		boxes.emplace_back(vmin, vmax);
	}
}

Material Level::loadMaterials(const aiMaterial* M)
{

	aiString Path;

	if (aiGetMaterialTexture(M, aiTextureType_BASE_COLOR, 0, &Path) == AI_SUCCESS)
	{
		const std::string albedoMap = std::string(Path.C_Str());
	}

	//	all other materials can be found with:
	//	aiTextureType_NORMAL_CAMERA, aiTextureType_METALNESS, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_AMBIENT_OCCLUSION

	return Material(Path.C_Str(), M->GetName().C_Str());
}

void Level::traverseTree(aiNode* n, Hierarchy* parent, Hierarchy* node)
{
	node->parent = parent;

	for (unsigned int i = 0; i < n->mNumMeshes; i++)
	{
		node->modelIndices.push_back(n->mMeshes[i]);
	}

	node->localTransform = toGlmMat4(n->mTransformation);

	for (size_t i = 0; i < n->mNumChildren; i++)
	{
		Hierarchy child;
		traverseTree(n->mChildren[i], node, &child);
		node->children.push_back(child);
	}


}

glm::mat4 Level::toGlmMat4(const aiMatrix4x4& mat)
{
	glm::mat4 result;
	result[0][0] = (float)mat.a1; result[0][1] = (float)mat.b1;  result[0][2] = (float)mat.c1; result[0][3] = (float)mat.d1;
	result[1][0] = (float)mat.a2; result[1][1] = (float)mat.b2;  result[1][2] = (float)mat.c2; result[1][3] = (float)mat.d2;
	result[2][0] = (float)mat.a3; result[2][1] = (float)mat.b3;  result[2][2] = (float)mat.c3; result[2][3] = (float)mat.d3;
	result[3][0] = (float)mat.a4; result[3][1] = (float)mat.b4;  result[3][2] = (float)mat.c4; result[3][3] = (float)mat.d4;
	return result;
}

void Level::setupVertexBuffers()
{
	glCreateBuffers(1, &VBO);
	glNamedBufferStorage(VBO, globalVertexOffset * 5 * sizeof(float), vertices.data(), 0);
	glCreateBuffers(1, &EBO);
	glNamedBufferStorage(EBO, globalIndexOffset * sizeof(GLuint), indices.data(), 0);

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

void Level::setupDrawBuffers()
{
	std::vector<glm::mat4> matrices;
	// setup buffers for drawing commands
	for (auto i = 0; i < models.size(); i++)
	{
		DrawElementsIndirectCommand icmd;
		icmd.count_ = 1;
		icmd.instanceCount_ = 1;
		icmd.firstIndex_ = meshes[models[i].meshIndex].indexOffset;
		icmd.baseVertex_ = meshes[models[i].meshIndex].vertexOffset;
		icmd.baseInstance_ = models[i].materialIndex + (uint32_t(i) << 16);
		drawCommands_.push_back(icmd);
	}

	glCreateBuffers(1, &IBO);
	glNamedBufferStorage(IBO, drawCommands_.size() * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(IBO, 0, drawCommands_.size() * sizeof(DrawElementsIndirectCommand), drawCommands_.data());

	glCreateBuffers(1, &matrixSSBO);
	//glNamedBufferStorage(matrixSSBO, matrices.size() * sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
	//glNamedBufferSubData(matrixSSBO, 0, matrices.size() * sizeof(glm::mat4), matrices.data());
	glNamedBufferStorage(matrixSSBO, sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
	 (matrixSSBO, 0, sizeof(glm::mat4), &glm::mat4(1)[0][0]);

}

void Level::loadLights(const aiScene* scene) {

	for (size_t i = 0; i < scene->mNumLights; i++)
	{
		const aiLightSourceType type = scene->mLights[i]->mType;

		if (type == aiLightSource_DIRECTIONAL)
		{
			const aiLight* light = scene->mLights[i];
			const aiVector3D dir = light->mDirection;
			const aiColor3D col = light->mColorDiffuse;

			glm::vec4 direction = glm::vec4(dir.x, dir.y, dir.z, 1.0f);
			glm::vec4 intensity = glm::vec4(col.r, col.g, col.b, 1.0f);

			lights.directional.push_back(DirectionalLight{direction, intensity});
		}

		if (type == aiLightSource_POINT) // (wrong position)
		{
			const aiLight* light = scene->mLights[i];
			const aiVector3D pos = light->mPosition;
			const aiColor3D col = light->mColorDiffuse;

			glm::vec4 position = glm::vec4(pos.x, pos.y, pos.z, 1.0f);
			glm::vec4 intensity = glm::vec4(col.r, col.g, col.b, 1.0f);

			lights.point.push_back(PositionalLight{ position, intensity });
		}
	}

	/*directional light
	lights.directional.push_back(DirectionalLight{
		glm::vec4(0.0f, 1.0f, 0.0f ,1.0f),		// direction
		glm::vec4(0.44f, 0.73f, 0.88f ,1.0f), });		// intensity 
	lights.directional.push_back(DirectionalLight{
		glm::vec4(0.0f, -1.0f, 0.0f ,1.0f),		// direction
		glm::vec4(0.92f, 0.47f, 0.26f ,1.0f), });		// intensity 

	// positional light
	lights.point.push_back(PositionalLight{
		glm::vec4(0.0f,  0.0f,  0.0f ,1.0f),		// position
		glm::vec4(1.0f, 1.0f, 1.0f ,1.0f) });		// intensity*/

}

void Level::Draw() const {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, matrixSSBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);	//TODO make buffer useful

	uint32_t boundMaterial = -1;


	// draw mesh
	glBindVertexArray(VAO);


	for (auto i = 0; i < models.size(); i++)
	{
		if (boundMaterial != models[i].materialIndex)
		{
			boundMaterial = models[i].materialIndex;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, materials[models[i].materialIndex].getAlbedo());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, materials[models[i].materialIndex].getNormalmap());

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, materials[models[i].materialIndex].getMetallic());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, materials[models[i].materialIndex].getRoughness());

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, materials[models[i].materialIndex].getAOmap());
		}

		GLsizei count = meshes[models[i].meshIndex].indexCount;
		GLint baseindex = meshes[models[i].meshIndex].indexOffset;


		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(GLint) * baseindex));
	}

	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // for debugging
}

void Level::DrawGraph() {
	//TODO make buffer useful
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, matrixSSBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);

	// draw mesh
	glBindVertexArray(VAO);
	drawTraverse(&sceneGraph, glm::mat4(1));
}


void Level::drawTraverse(const Hierarchy* node, glm::mat4 globalTransform)
{
	glm::mat4 modelMatrix = node->localTransform * globalTransform;

	for (size_t i = 0; i < node->modelIndices.size(); i++)
	{
		uint32_t modelIndex = node->modelIndices[i];

		if (boundMaterial != models[modelIndex].materialIndex)
		{
			boundMaterial = models[modelIndex].materialIndex;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, materials[models[modelIndex].materialIndex].getAlbedo());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, materials[models[modelIndex].materialIndex].getNormalmap());

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, materials[models[modelIndex].materialIndex].getMetallic());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, materials[models[modelIndex].materialIndex].getRoughness());

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, materials[models[modelIndex].materialIndex].getAOmap());
		}

		glNamedBufferSubData(matrixSSBO, 0, sizeof(glm::mat4), &modelMatrix[0][0]);

		GLsizei count = meshes[models[modelIndex].meshIndex].indexCount;
		GLint baseindex = meshes[models[modelIndex].meshIndex].indexOffset;

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(GLint) * baseindex));
	}

	for (size_t i = 0; i < node->children.size(); i++)
	{
		drawTraverse(&node->children[i], modelMatrix);
	}
}

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