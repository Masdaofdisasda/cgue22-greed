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
	void swapLuminance();

	GlobalState static loadSettings(GlobalState state);
private:
	GlobalState* globalState;
	PerFrameData* perframeData;
	PerFrameSettings perframsets;
	LightSources lights;
	UBO directionalLights;
	UBO positionalLights;
	UBO spotLights;
	UBO perframeBuffer;
	UBO perframesetBuffer;
	Program PBRShader;
	Program skyboxShader;
	Program BrightPass;
	Program ToLuminance;
	Program BlurX;
	Program BlurY;
	Program CombineHDR;
	Program lightAdapt;

	// Framebuffers for Bloom
	GLuint luminance1x1;
	// Framebuffer size cant be changed after init eg. window reisizing not correctly working
	Framebuffer framebuffer = Framebuffer(1920, 1080, GL_RGBA16F, GL_DEPTH_COMPONENT24);
	Framebuffer luminance = Framebuffer(64, 64, GL_R16F, 0);
	Framebuffer brightPass = Framebuffer(256, 256, GL_RGBA16F, 0);
	Framebuffer bloom1 = Framebuffer(256, 256, GL_RGBA16F, 0);
	Framebuffer bloom2 = Framebuffer(256, 256, GL_RGBA16F, 0);
	Texture luminance1 = Texture(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	Texture luminance2 = Texture(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	const Texture* luminances[2] = { &luminance1, &luminance2 };
	const glm::vec4 brightPixel = glm::vec4(glm::vec3(50.0f), 1.0f);

	void fillLightsources();
	void buildShaderPrograms(); 
	void prepareFramebuffers();
	void setRenderSettings();
};


