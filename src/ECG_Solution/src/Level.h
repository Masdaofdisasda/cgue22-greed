#pragma once
#include <vector>
#include "Mesh.h"
#include "LightSource.h"
#include "Camera.h"

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

//---------------------------------------------------------------------------------------------------------------//
class ModelTesterLevel : public LevelInterface {
private:
	//TODO
	//load fbx files with assimp, auto import materials (textures) from fbx
	// and create a single vertex array and a single element array

	LightSources lights;

	void loadMaterials() {

		//TODO

	}

	void loadModels() {

		//TODO

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
		loadMaterials();
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