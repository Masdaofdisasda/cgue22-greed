#pragma once

#include "Utils.h"
#include "Program.h"
#include "Texture.h"
#include "Cubemap.h"
#include "LightSource.h"
#include "UBO.h"

class Renderer
{
public:
	Renderer(GlobalState& state, PerFrameData& pfdata, LightSources lights);
	~Renderer();

	void Draw(std::vector <Mesh*> models, Mesh& skybox);

	GlobalState static loadSettings(GlobalState state);
private:
	GlobalState* globalState;
	PerFrameData* perframeData;
	LightSources lights;
	UBO directionalLights;
	UBO positionalLights;
	UBO spotLights;
	UBO perframeBuffer;
	Program PBRShader;
	Program skyboxShader;

	void fillLightsources();
	void buildShaderPrograms();
};


