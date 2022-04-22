#pragma once

#include "Program.h"
#include "Texture.h"
#include "Cubemap.h"
#include "LightSource.h"
#include "UBO.h"
#include "Framebuffer.h"
#include "Level.h"
#include "FontRenderer.h"
#include "Lava.h"

#ifndef _RENDERER_
#define _RENDERER_
class Renderer
{
public:
	Renderer(PerFrameData& pfdata, LightSources& sources);
	~Renderer();

	void Draw(Level* level);
	void swapLuminance();

	GlobalState static loadSettings();
	std::shared_ptr<GlobalState> static getState();
	static std::shared_ptr<GlobalState> state;
private:
	// Render Settings
	PerFrameData* perframeData;	// viewproj, viewpos,...
	UBO perframeBuffer;	

	FontRenderer fontRenderer;
	LavaSystem lavaSim;

	// Illumination
	LightSources lights;
	UBO directionalLights;
	UBO positionalLights;

	// Shader Programs
	// Scene rendering
	Program PBRShader;		// main illumination shader
	Program skyboxShader;	// simple skybox shader
	Program lavaFloor;		// renders a giant orange triangle
	// Bloom/HDR
	Program BrightPass;		// filter bright spots
	Program ToLuminance;	// converts brightness
	Program BlurX;			// gauss blur in x direction
	Program BlurY;			// gauss blur in y direction
	Program CombineHDR;		// combines blur with render fbo, tone mapping
	Program lightAdapt;		// calculates luminance changes
	// SSAO
	Program SSAO;			// calculates occlusion
	Program CombineSSAO;	// combines ssao with render fbo
	// Volumetric Light
	Program DepthMap;		// samples depth from a directional light
	Program VolumetricLight;// calculate light with raymarching
	Program downsampleVL;	// downsamples depth buffer
	Program upsampleVL;		// upsamples depth buffer
	Program blurXVL;
	Program blurYVL;
	// HUD
	Program renderImage;
	Program renderColor;

	// global Textures
	Cubemap IBL;
	Cubemap skyTex;
	GLuint Lut3D;
	GLuint emptyVAO;		// for skybox drawing

	// Framebuffers for HDR/Bloom
	GLuint luminance1x1;
	// Framebuffer size cant be changed after init eg. window resizing not correctly working
	Framebuffer framebuffer1 = Framebuffer(state->width, state->height, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
	Framebuffer framebuffer2 = Framebuffer(state->width, state->height, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
	Framebuffer framebuffer3 = Framebuffer(state->width, state->height, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
	Framebuffer luminance = Framebuffer(64, 64, GL_RGBA16F, 0);
	Framebuffer brightPass = Framebuffer(256, 256, GL_RGBA16F, 0);
	Framebuffer bloom0 = Framebuffer(256, 256, GL_RGBA16F, 0); 
	Framebuffer bloom1 = Framebuffer(256, 256, GL_RGBA16F, 0);
	Texture luminance0 = Texture(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	Texture luminance1 = Texture(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	const Texture* luminances[2] = { &luminance0, &luminance1 };

	// Framebuffers for SSAO
	Framebuffer ssao = Framebuffer(1024, 1024, GL_RGBA8, 0);
	Framebuffer blur = Framebuffer(1024, 1024, GL_RGBA8, 0);
	GLuint pattern;

	// Framebuffers for light/shadow
	Framebuffer depthMap = Framebuffer(1024 * state->shadowRes_, 1024 * state->shadowRes_, 0, GL_DEPTH_COMPONENT24);
	Framebuffer blur0 = Framebuffer(state->width / 2, state->height / 2, GL_RGBA8, 0);
	Framebuffer blur1 = Framebuffer(state->width / 2, state->height / 2, GL_RGBA8, 0);
	Framebuffer depthHalfRes = Framebuffer(state->width / 2, state->height / 2, 0, GL_DEPTH_COMPONENT24);

	GLuint hud;

	void fillLightsources();
	void buildShaderPrograms(); 
	void prepareFramebuffers();
	void setRenderSettings();
	glm::mat4 glmlookAt2(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
};
#endif


