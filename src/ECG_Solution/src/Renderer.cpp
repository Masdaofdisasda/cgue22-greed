#include "Renderer.h"

Renderer::Renderer(GlobalState& state, PerFrameData& pfdata, LightSources lights)
{
	globalState = &state; // link global variables
	perframeData = &pfdata; // link per frame data
	this->lights = lights; // set lights and lightcounts for shaders
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

void Renderer::fillLightsources()
{
	// create Uniform Buffer Objects from light source struct vectors
	directionalLights.fillBuffer(lights.directional);
	positionalLights.fillBuffer(lights.point);
	spotLights.fillBuffer(lights.spot);

	// bind UBOs to bindings in shader
	PBRShader.bindLightBuffers(&directionalLights, &positionalLights, &spotLights);
	// set light source count variables
	PBRShader.setuInt("dLightCount", lights.directional.size());
	PBRShader.setuInt("pLightCount", lights.point.size());
	PBRShader.setuInt("sLightCount", lights.spot.size());
}

void Renderer::buildShaderPrograms()
{
	// build shader programms
	Shader pbrVert("assets/shaders/pbr/pbr.vert");
	Shader pbrFrag("assets/shaders/pbr/pbr.frag", glm::ivec3(lights.directional.size(), lights.point.size(), lights.spot.size()));
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