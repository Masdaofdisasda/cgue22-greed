#pragma once
#include <vector>
#include "Mesh.h"
#include "LightSource.h"
#include "Camera.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

/*
Abstract class for all Level classes.
Helps with seperating different scenes with different models, lights, cameras and controls.
Level data could also be read from a file.
*/
class LevelInterface {
public:
	virtual std::vector <Mesh>* getModels() = 0;
	virtual LightSources getLights() = 0;
	virtual std::vector <DirectionalLight> getDirectionalLights() = 0;
	virtual std::vector <PositionalLight> getPointLights() = 0;
};

struct MeshDraw // data for drawing
{
	uint32_t meshIndex;
	uint32_t materialID;		// material identifier
	uint32_t indexOffset;
	uint32_t vertexOffset;
	uint32_t transformIndex;
};

struct MeshObj // mesh object
{
	uint32_t streamCount;
	uint32_t indexOffset;
	uint32_t vertexOffset;		
	uint32_t vertexCount;
	uint32_t streamOffset;
	uint32_t streamElementSize;
};

//---------------------------------------------------------------------------------------------------------------//
class ModelTesterLevel : public LevelInterface {
private:
	//TODO
	//load fbx files with assimp, auto import materials (textures) from fbx
	// and create a single vertex array and a single element array
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint EBO = 0;
	std::vector <MeshObj> meshes; 
	std::vector<float> vertices; 
	std::vector <GLuint> indices;

	LightSources lights;

	void loadMaterials() {

		//TODO

	}

	void loadModels() {

		std::cout << "load scene... (this could take a few minutes)"<< std::endl;
		// load fbx file into assimps internal data structures
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile("assets/Bistro_v5_2/BistroInterior.fbx", 
			aiProcess_MakeLeftHanded | 
			aiProcess_FlipWindingOrder |
			aiProcess_FlipUVs | 
			aiProcess_PreTransformVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_Triangulate |
			aiProcess_FixInfacingNormals |
			aiProcess_FindInvalidData |
			aiProcess_ValidateDataStructure | 0);

		std::cout << "done loading scene... " << std::endl;

		if (!scene) // check if the scene was actually loaded
		{
			std::cerr << "ERROR: Couldn't load scene" << std::endl;
		}

		uint32_t globalVertexOffset = 0;
		uint32_t globalIndexOffset = 0;

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			MeshObj m;
			m.streamCount = 1;
			m.indexOffset = globalIndexOffset;
			m.vertexOffset = globalVertexOffset;
			m.vertexCount = mesh->mNumVertices;
			m.streamElementSize = static_cast<uint32_t>((3+2+3) * sizeof(float)); //pos,uv,normal
			m.streamOffset = m.vertexOffset * m.streamElementSize;

			// extract vertices
			for(size_t j = 0; j < mesh->mNumVertices; j++)
			{
			std::cout << "load mesh " << i << " of " << scene->mNumMeshes << " - progress: " << j/(float)mesh->mNumVertices << std::endl;
				const aiVector3D p = mesh->HasPositions() ? mesh->mVertices[j] : aiVector3D();
				const aiVector3D n = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D();
				const aiVector3D t = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D();

				vertices.push_back(p.x);
				vertices.push_back(p.y);
				vertices.push_back(p.z);

				vertices.push_back(t.x);
				vertices.push_back(t.y);

				vertices.push_back(n.x);
				vertices.push_back(n.y);
				vertices.push_back(n.z);
			}

			//extract indices
			for (size_t j = 0; j < mesh->mNumFaces; j++)
			{
				if (mesh->mFaces[j].mNumIndices != 3)
					continue;
				for (unsigned k = 0; k != mesh->mFaces[j].mNumIndices; k++)
					indices.push_back(mesh->mFaces[j].mIndices[k]);
			}

			globalVertexOffset += mesh->mNumVertices;
			globalIndexOffset += indices.size() - globalIndexOffset;

			meshes.push_back(m);

		}

		glCreateBuffers(1, &VBO);
		glNamedBufferStorage(VBO, globalVertexOffset * 5 * sizeof(float), vertices.data(), 0);
		glCreateBuffers(1, &EBO);
		glNamedBufferStorage(EBO, globalIndexOffset * sizeof(GLuint), indices.data(), 0);

		glCreateVertexArrays(1, &VAO);
		glVertexArrayElementBuffer(VAO, EBO);
		glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec3));
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
		glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3) + sizeof(glm::vec3));
		glVertexArrayAttribBinding(VAO, 2, 0);

	}

	void loadLights() {
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

public:
	ModelTesterLevel() {
		loadModels();
		loadLights();
	}
	~ModelTesterLevel() {}

	std::vector <Mesh>* getModels() { return nullptr; }

	Mesh* getSkybox() { return nullptr; }

	LightSources getLights() { return lights; }

	std::vector <DirectionalLight> getDirectionalLights() { return lights.directional; }
	std::vector <PositionalLight> getPointLights() { return lights.point; }
};

//---------------------------------------------------------------------------------------------------------------//
class PhysicsTesterLevel : public LevelInterface {
private:
	std::vector <Mesh> models;
	Mesh* skybox = nullptr;
	LightSources lights;

	void loadModels() {
		// create textures
		//Texture* brickDiff = new Texture("assets/textures/brick03-diff.jpeg");
		//Texture* brickSpec = new Texture("assets/textures/brick03-spec.jpeg");
		//Cubemap* brickCube = new Cubemap("assets/textures/cubemap");

		//// create materials
		//Material brick(brickDiff, brickSpec, brickCube,
		//	glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), 1.0f);

		//// create meshes
		//Mesh fallingCube = Mesh::Cube(1, 1, 1, &brick);
		//Mesh floor = Mesh::Cube(100, 100, .5f, &brick);

		//// move meshes
		//fallingCube.translate(glm::vec3(0.0f, 0.0f, 10.0f));

		//// add meshes to models
		//models.push_back(&fallingCube);
		//models.push_back(&floor);
	}
	void loadPhysics() {
		////Bullet Initialization
		//btDbvtBroadphase* broadphase = new btDbvtBroadphase();
		//btDefaultCollisionConfiguration* collision_configuration = new btDefaultCollisionConfiguration();
		//btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collision_configuration);
		//btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		//btDiscreteDynamicsWorld* dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
		//dynamics_world->setGravity(btVector3(0, -10, 0));

		//btQuaternion* rotation = new btQuaternion(0.0, 0.0, 0.0);
		//btVector3* position = new btVector3(0.0, 5.0, 0.0);
		//btTransform* cubestartTrans = new btTransform(*rotation, *position);
		//btTransform* cubeCenterOfMassOffset = new btTransform();
		//btVector3* boxSize = new btVector3(.5, .5, .5);
		//btRigidBody fallingCube(
		//	1.0,
		//	new btDefaultMotionState(*cubestartTrans, *cubeCenterOfMassOffset),
		//	new btBoxShape(*boxSize)
		//);

		//dynamics_world->addRigidBody(&fallingCube);

	}
	void loadSkybox() {
		/*Texture brickDiff("assets/textures/brick03-diff.jpeg");
		Texture brickSpec("assets/textures/brick03-spec.jpeg");
		Cubemap brickCube("assets/textures/cubemap");

		Material sky(&brickDiff, &brickSpec, &brickCube,
			glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);

		Mesh skybox = skybox.Skybox(400.0f, &sky);*/
	}
	void loadLights() {
		// directional light
		lights.directional.push_back(DirectionalLight{
			glm::vec4(0.0f, -1.0f, -1.0f ,1.0f),		// direction
			glm::vec4(0.8f, 0.8f, 0.8f ,1.0f), });		// intensity 
	}

public:
	PhysicsTesterLevel() {
		loadModels();
		loadSkybox();
		loadLights();
		loadPhysics();
	}
	~PhysicsTesterLevel() {}

	std::vector <Mesh>* getModels() { return &models; }

	Mesh* getSkybox() { return skybox; }

	LightSources getLights() { return lights; }

	std::vector <DirectionalLight> getDirectionalLights() { return lights.directional; }
	std::vector <PositionalLight> getPointLights() { return lights.point; }
};