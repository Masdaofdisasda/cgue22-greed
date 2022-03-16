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
	virtual Mesh* getModels() = 0;
	virtual Mesh* getSkybox() = 0;
	virtual LightSources getLights() = 0;
	virtual std::vector <DirectionalLight> getDirectionalLights() = 0;
	virtual std::vector <PositionalLight> getPointLights() = 0;
	virtual std::vector <SpotLight> getSpotlights() = 0;
};

class ModelTesterLevel : public LevelInterface {
private:
	std::vector <Mesh*> models;
	Mesh* skybox = nullptr;

	LightSources lights;

	void loadModels() {

		std::cout << "initialize models and textures..." << std::endl;

		//model1.translate(glm::vec3(1.0f, -1.0f, -5.0f));
		//model2.translate(glm::vec3(-1.0f, 1.0f, -5.0f));


	}

	void loadLights() {
		// directional light
		lights.directional.push_back(DirectionalLight{
			glm::vec4(0.0f, -1.0f, -1.0f ,1.0f),		// direction
			glm::vec4(0.8f, 0.8f, 0.8f ,1.0f), });		// intensity 

		// positional light
		lights.point.push_back(PositionalLight{
			glm::vec4(0.0f,  0.0f,  0.0f ,1.0f),		// position
			glm::vec4(1.0f, 0.4f, 0.1f, 1.0f),			// attenuation (constant, linear, quadratic)
			glm::vec4(1.0f, 1.0f, 1.0f ,1.0f) });		// intensity


		// spot light
		lights.spot.push_back(SpotLight{
			glm::vec4(5.0f,-4.0f,-2.0f,1.0f), // position
			glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
			glm::vec4(glm::cos(glm::radians(7.0f)),glm::cos(glm::radians(5.0f)),1.0f,1.0f), // angles (outer, inner)
			glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)
			glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

		lights.spot.push_back(SpotLight{
			glm::vec4(2.5f,-4.0f,-2.0f,1.0f), // position
			glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
			glm::vec4(glm::cos(glm::radians(14.0f)),glm::cos(glm::radians(10.0f)),1.0f,1.0f), // angles (outer, inner)
			glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)
			glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

		lights.spot.push_back(SpotLight{
			glm::vec4(0.0f,-4.0f,-2.0f,1.0f), // position
			glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
			glm::vec4(glm::cos(glm::radians(28.0f)),glm::cos(glm::radians(20.0f)),1.0f,1.0f), // angles (outer, inner)
			glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)
			glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

		lights.spot.push_back(SpotLight{
			glm::vec4(-2.5f,-4.0f,-2.0f,1.0f), // position
			glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
			glm::vec4(glm::cos(glm::radians(56.0f)),glm::cos(glm::radians(40.0f)),1.0f,1.0f), // angles (outer, inner)
			glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)
			glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

		lights.spot.push_back(SpotLight{
			glm::vec4(-5.0f,-4.0f,-2.0f,1.0f), // position
			glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
			glm::vec4(glm::cos(glm::radians(112.0f)),glm::cos(glm::radians(80.0f)),1.0f,1.0f), // angles (outer, inner)
			glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)
			glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity
	}

public:
	ModelTesterLevel() {
		loadModels();
		loadLights();
	}
	~ModelTesterLevel() {}

	Mesh* getModels() { return nullptr; }

	Mesh* getSkybox() { return nullptr; }

	LightSources getLights() { return lights; }

	std::vector <DirectionalLight> getDirectionalLights() { return lights.directional; }
	std::vector <PositionalLight> getPointLights() { return lights.point; }
	std::vector <SpotLight> getSpotlights() { return lights.spot; }
};