#pragma once

#include "Program.h"
#include "Texture.h"
#include "Cubemap.h"
#include "LightSource.h"
#include "UBO.h"
#include "Framebuffer.h"
#include "Level.h"

#ifndef _RENDERER_
#define _RENDERER_
class Renderer
{
public:
	Renderer(GlobalState& state, PerFrameData& pfdata, LightSources& sources);
	~Renderer();

	void Draw(Level* level);
	void swapLuminance();

	GlobalState static loadSettings();
private:
	// Render Settings
	GlobalState* globalState;
	PerFrameData* perframeData;	// viewproj, viewpos,...
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
	Program lightAdapt;		// calculates luminance changes
	Program lavaFloor;		// renders a giant orange triangle
	Program SSAO;
	Program CombineSSAO;
	Program renderImage;

	// global Textures
	Cubemap IBL;
	Cubemap skyTex;
	GLuint emptyVAO;		// for skybox drawing

	// Framebuffers for HDR/Bloom
	GLuint luminance1x1;
	// Framebuffer size cant be changed after init eg. window reisizing not correctly working
	Framebuffer framebuffer1 = Framebuffer(1920, 1080, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
	Framebuffer framebuffer2 = Framebuffer(1920, 1080, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
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

	GLuint hud;

	void fillLightsources();
	void buildShaderPrograms(); 
	void prepareFramebuffers();
	void setRenderSettings();
};
#endif


