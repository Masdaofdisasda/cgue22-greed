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

	void Draw(std::vector <Mesh*> models);
	void swapLuminance();

	GlobalState static loadSettings(GlobalState state);
private:
	// Render Settings
	GlobalState* globalState;
	PerFrameData* perframeData;	// viewproj, viewpos,...
	PerFrameSettings perframsets; // effect settings
	UBO perframeBuffer;	
	UBO perframesetBuffer;

	// Illumination
	LightSources lights;
	UBO directionalLights;
	UBO positionalLights;

	// Shader Programs
	Program PBRShader;		// main illumination shader
	Program skyboxShader;	// simple skybox shader
	Program BrightPass;		// filter bright spots
	Program ToLuminance;	// converts brightness
	Program BlurX;			// gauss blur in x direction
	Program BlurY;			// gauss blur in y direction
	Program CombineHDR;		// combines blur with render fbo, tone mapping
	Program lightAdapt;		// controls exposure changes

	// global Textures
	Cubemap IBL;
	Cubemap skyTex;
	Mesh skyBox = skyBox.Skybox();

	// Framebuffers for HDR/Bloom
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


