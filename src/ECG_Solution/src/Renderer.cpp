#include "Renderer.h"

Renderer::Renderer(GlobalState& state, PerFrameData& pfdata)
{
	globalState = &state; // link global variables
	perframeData = &pfdata; // link per frame data
	loadLightsources(); // load lights and lightcounts for shaders
	std::cout << "fail..." << std::endl;
	buildShaderPrograms(); // build shader program
	fillLightsources(); // binds lights to biding points in shader
	perframeBuffer.fillBuffer(pfdata); // load buffer to shader;

}

GlobalState Renderer::loadSettings(GlobalState state)
{
	// init reader for ini files
	std::cout << "reading setting from settings.ini..." << std::endl;
	INIReader reader("assets/settings.ini");
	// load values from ini file
	// first param: section [window], second param: property name, third param: default value
	state.width = reader.GetInteger("window", "width", 800);
	state.height = reader.GetInteger("window", "height", 800);
	state.refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	state._fullscreen = reader.GetBoolean("window", "fullscreen", false);
	state.window_title = reader.Get("window", "title", "ECG 2021");
	state.fov = reader.GetReal("camera", "fov", 60.0f);
	state.Znear = reader.GetReal("camera", "near", 0.1f);
	state.Zfar = reader.GetReal("camera", "far", 100.0f);

	return state;

}

void Renderer::loadLightsources()
{
	// directional light
	dLightsBuffer.push_back(DirectionalLight{
		glm::vec4(0.0f, -1.0f, -1.0f ,1.0f),		// direction

		glm::vec4(0.8f, 0.8f, 0.8f ,1.0f), });		// intensity 

	// positional light
	pLightsBuffer.push_back(PositionalLight{
		glm::vec4(0.0f,  0.0f,  0.0f ,1.0f),		// position
		glm::vec4(1.0f, 0.4f, 0.1f, 1.0f),			// attenuation (constant, linear, quadratic)

		glm::vec4(1.0f, 1.0f, 1.0f ,1.0f) });		// intensity


	// spot light
	sLightsBuffer.push_back(SpotLight{
		glm::vec4(5.0f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(7.0f)),glm::cos(glm::radians(5.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

	sLightsBuffer.push_back(SpotLight{
		glm::vec4(2.5f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(14.0f)),glm::cos(glm::radians(10.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

	sLightsBuffer.push_back(SpotLight{
		glm::vec4(0.0f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(28.0f)),glm::cos(glm::radians(20.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

	sLightsBuffer.push_back(SpotLight{
		glm::vec4(-2.5f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(56.0f)),glm::cos(glm::radians(40.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity
	sLightsBuffer.push_back(SpotLight{
		glm::vec4(-5.0f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(112.0f)),glm::cos(glm::radians(80.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity


}

void Renderer::fillLightsources()
{
	// create Uniform Buffer Objects from light source struct vectors
	directionalLights.fillBuffer(dLightsBuffer);
	positionalLights.fillBuffer(pLightsBuffer);
	spotLights.fillBuffer(sLightsBuffer);

	// bind UBOs to bindings in shader
	PBRShader.bindLightBuffers(&directionalLights, &positionalLights, &spotLights);
	// set light source count variables
	PBRShader.setuInt("dLightCount", dLightsBuffer.size());
	PBRShader.setuInt("pLightCount", pLightsBuffer.size());
	PBRShader.setuInt("sLightCount", sLightsBuffer.size());
}

void Renderer::buildShaderPrograms()
{
	// build shader programms
	Shader pbrVert("assets/shaders/pbr/pbr.vert");
	Shader pbrFrag("assets/shaders/pbr/pbr.frag", glm::ivec3(dLightsBuffer.size(), pLightsBuffer.size(), sLightsBuffer.size()));
	PBRShader.buildFrom(pbrVert, pbrFrag);
	PBRShader.Use();

	Shader skyboxVert("assets/shaders/skybox/skybox.vert");
	Shader skyboxFrag("assets/shaders/skybox/skybox.frag");
	skyboxShader.buildFrom(skyboxVert, skyboxFrag);

}

void Renderer::Draw(std::vector <Mesh*> models, Mesh& skybox)
{
	perframeBuffer.Update(*perframeData);

	PBRShader.Use();
	for (auto& model : models)
	{
		PBRShader.Draw(*model);
	}

	// draw skybox    
	skyboxShader.Use();
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Draw(skybox);
	glDepthFunc(GL_LESS);
}

Renderer::~Renderer()
{
	globalState = nullptr;
	perframeData = nullptr;
}