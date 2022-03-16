#pragma once

#include "Program.h"
#include "Texture.h"
#include "Cubemap.h"
#include "LightSource.h"
#include "UBO.h"
#include "Framebuffer.h"

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
	Program blurShader;
	Program bloomShader;

	// Framebuffers for Bloom
	Framebuffer framebuffer = Framebuffer(800, 800, GL_RGBA16F, GL_DEPTH_COMPONENT24);
	Framebuffer luminance = Framebuffer(64, 64, GL_R16F, 0);
	Framebuffer brightPass = Framebuffer(256, 256, GL_RGBA16F, 0);
	Framebuffer bloom1 = Framebuffer(256, 256, GL_RGBA16F, 0);
	Framebuffer bloom2 = Framebuffer(256, 256, GL_RGBA16F, 0);

	void fillLightsources();
	void buildShaderPrograms(); 
	void prepareFramebuffers();
};


