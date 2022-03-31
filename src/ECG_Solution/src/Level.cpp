#include "Level.h"

Level::Level(const char* scenePath) {

	// 1. load fbx file into assimps internal data structures and apply various preprocessing to the data
	std::cout << "load scene... (this could take a while)" << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("assets/Bistro_v5_2/BistroInterior.fbx",
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_FlipUVs |
		aiProcess_FixInfacingNormals |
		aiProcess_GenSmoothNormals |
		aiProcess_PreTransformVertices |
		aiProcess_ValidateDataStructure);

	if (!scene)
	{
		std::cerr << "ERROR: Couldn't load scene" << std::endl;
	}

	globalVertexOffset = 0;
	globalIndexOffset = 0;

	meshes.reserve(scene->mNumMeshes);
	boxes.reserve(scene->mNumMeshes);

	// 2. iterate through the scene and create a mesh object for every aimesh in the scene
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		std::cout << "load mesh " << i * 100 / scene->mNumMeshes << "%" << std::endl;
		const aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(extractMesh(mesh));

	}

	std::vector<std::string> files;

	// load materials
	for (unsigned int m = 0; m < scene->mNumMaterials; m++)
	{
		aiMaterial* mm = scene->mMaterials[m];

		printf("Material [%s] %u\n", mm->GetName().C_Str(), m);

		Material mat = loadMaterials(mm, files);
		materials.push_back(mat);
	}
	//load transformation matrices
	transforms.push_back(glm::mat4(1));


	// 3. create a model for drawing for every loaded mesh
	for (auto i = 0; i < meshes.size(); i++)
	{
		drawData draw;
		draw.meshIndex = (uint32_t)i;
		draw.materialIndex = 0;
		draw.transformIndex = 0;
		models.push_back(draw);

	}

	// 4. setup buffers for vertex and indices data
	setupVertexBuffers();

	// 5. setup buffers for transform and drawcommands
	setupDrawBuffers(); //TODO

	// 6. load lights sources 
	loadLights(); //TODO
}

MeshObj Level::extractMesh(const aiMesh* mesh)
{
	MeshObj m;
	m.indexOffset = globalIndexOffset;
	m.vertexOffset = globalVertexOffset;
	m.vertexCount = mesh->mNumVertices;

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

	std::cout << "Mesh " << meshes.size() << " - " << mesh->mName.C_Str() << std::endl;
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

Material Level::loadMaterials(const aiMaterial* M, std::vector<std::string>& files)
{

	return Material("assets/textures/coin");
	aiString Path;

	if (aiGetMaterialTexture(M, aiTextureType_DIFFUSE, 0, &Path) == AI_SUCCESS)
	{
		const std::string albedoMap = std::string(Path.C_Str());
		std::cout << albedoMap << std::endl;
	}
	if (aiGetMaterialTexture(M, aiTextureType_NORMALS, 0, &Path) == AI_SUCCESS)
	{
		const std::string normal = std::string(Path.C_Str());
		std::cout << normal << std::endl;
	}
	if (aiGetMaterialTexture(M, aiTextureType_METALNESS, 0, &Path) == AI_SUCCESS)
	{
		const std::string metal = std::string(Path.C_Str());
		std::cout << metal << std::endl;
	}
	if (aiGetMaterialTexture(M, aiTextureType_DIFFUSE_ROUGHNESS, 0, &Path) == AI_SUCCESS)
	{
		const std::string rough = std::string(Path.C_Str());
		std::cout << rough << std::endl;
	}
	if (aiGetMaterialTexture(M, aiTextureType_AMBIENT_OCCLUSION, 0, &Path) == AI_SUCCESS)
	{
		const std::string ao = std::string(Path.C_Str());
		std::cout << ao << std::endl;
	}

	// TODO load textures from path

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
	// uv
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
	glVertexArrayAttribBinding(VAO, 1, 0);
	// normal
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

		matrices.push_back(transforms[models[i].transformIndex]);
	}

	glCreateBuffers(1, &IBO);
	glNamedBufferStorage(IBO, drawCommands_.size() * sizeof(DrawElementsIndirectCommand), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(IBO, 0, drawCommands_.size() * sizeof(DrawElementsIndirectCommand), drawCommands_.data());

	glCreateBuffers(1, &matrixSSBO);
	glNamedBufferStorage(matrixSSBO, matrices.size() * sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(matrixSSBO, 0, matrices.size() * sizeof(glm::mat4), matrices.data());

	glCreateBuffers(1, &materialSSBO);
	glNamedBufferStorage(materialSSBO, materials.size() * sizeof(Material), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(materialSSBO, 0, materials.size() * sizeof(Material), materials.data());

}

void Level::loadLights() {
	// directional light
	lights.directional.push_back(DirectionalLight{
		glm::vec4(0.0f, 1.0f, 0.0f ,1.0f),		// direction
		glm::vec4(0.44f, 0.73f, 0.88f ,1.0f), });		// intensity 
	lights.directional.push_back(DirectionalLight{
		glm::vec4(0.0f, -1.0f, 0.0f ,1.0f),		// direction
		glm::vec4(0.92f, 0.47f, 0.26f ,1.0f), });		// intensity 

	// positional light
	lights.point.push_back(PositionalLight{
		glm::vec4(0.0f,  0.0f,  0.0f ,1.0f),		// position
		glm::vec4(1.0f, 1.0f, 1.0f ,1.0f) });		// intensity

}

void Level::Draw() const {
	//TODO make buffer useful
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materialSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, matrixSSBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);

	uint32_t boundMaterial = -1;


	// draw mesh
	glBindVertexArray(VAO);


	for (auto i = 0; i < models.size(); i++)
	{
		if (boundMaterial != models[i].materialIndex)
		{
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
		GLint basevertex = meshes[models[i].meshIndex].vertexOffset;


		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(GLint) * baseindex));
	}

	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}