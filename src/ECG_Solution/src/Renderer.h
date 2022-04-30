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

class renderer
{
public:
	/// @brief sets up shaders for rendering and post processing and also enviroment maps
	/// @param perframe_data is per render cycle static data
	/// @param sources are the light sources of some level
	renderer(PerFrameData& perframe_data, light_sources& sources);
	~renderer();

	/**
	 * \brief run through the render pipeline
	 * \param level that should get rendered
	 */
	void draw(level* level);
	void swap_luminance();

	std::shared_ptr<GlobalState> static get_state();
	static std::shared_ptr<GlobalState> state;

private:
	// Render Settings
	PerFrameData* perframe_data_;	// viewproj, viewpos,...
	UBO perframe_buffer_;	

	font_renderer font_renderer_;
	lava_system lava_sim_;

	// Illumination
	light_sources lights_;
	UBO directional_lights_, positional_lights_;

	// Shader Programs
	// Scene rendering
	program pbr_shader_;		// main illumination shader
	program skybox_shader_;		// simple skybox shader
	// Bloom/HDR
	program bright_pass_;		// filter bright spots
	program to_luminance_;		// converts brightness
	program blur_x_;			// gauss blur in x direction
	program blur_y_;			// gauss blur in y direction
	program combine_hdr_;		// combines blur with render fbo, tone mapping
	program light_adapt_;		// calculates luminance changes
	// SSAO
	program ssao_;				// calculates occlusion
	program combine_ssao_;		// combines ssao with render fbo
	// Volumetric Light
	program depth_map_;			// samples depth from a directional light
	program volumetric_light_;	// calculate light with raymarching
	program downsample_vl_;		// downsamples depth buffer
	program upsample_vl_;		// upsamples depth buffer
	program blur_xvl_;
	program blur_yvl_;
	// HUD
	program render_image_;		// draw image to full viewport
	program render_color_;		// draw color to full viewport

	// global Textures
	cubemap ibl_, sky_tex_;
	GLuint lut_3d_ = 0;
	GLuint empty_vao_ = 0;		// for skybox drawing

	// Framebuffers for HDR/Bloom
	GLuint luminance1x1_{};
	// Framebuffer size cant be changed after init eg. window resizing not correctly working
	framebuffer framebuffer1_ = framebuffer(state->width, state->height, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
	framebuffer framebuffer2_ = framebuffer(state->width, state->height, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
	framebuffer framebuffer3_ = framebuffer(state->width, state->height, GL_RGBA16F, GL_DEPTH_COMPONENT24); // main render target for processing
	framebuffer luminance_ = framebuffer(64, 64, GL_RGBA16F, 0);
	framebuffer bright_pass_fb_ = framebuffer(256, 256, GL_RGBA16F, 0);
	framebuffer bloom0_ = framebuffer(256, 256, GL_RGBA16F, 0); 
	framebuffer bloom1_ = framebuffer(256, 256, GL_RGBA16F, 0);
	Texture luminance0_ = Texture(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	Texture luminance1_ = Texture(GL_TEXTURE_2D, 1, 1, GL_RGBA16F);
	const Texture* luminances_[2] = { &luminance0_, &luminance1_ };

	// Framebuffers for SSAO
	framebuffer ssao_fb_ = framebuffer(1024, 1024, GL_RGBA8, 0);
	framebuffer blur_ = framebuffer(1024, 1024, GL_RGBA8, 0);
	GLuint pattern_ = 0;

	// Framebuffers for light/shadow
	framebuffer depth_map_fb_ = framebuffer(1024 * state->shadowRes_, 1024 * state->shadowRes_, 0, GL_DEPTH_COMPONENT24);
	framebuffer blur0_ = framebuffer(state->width / 2, state->height / 2, GL_RGBA8, 0);
	framebuffer blur1_ = framebuffer(state->width / 2, state->height / 2, GL_RGBA8, 0);
	framebuffer depth_half_res_ = framebuffer(state->width / 2, state->height / 2, 0, GL_DEPTH_COMPONENT24);

	GLuint hud_ = 0;

	std::vector<std::string> death_msgs_ =
	{
		"Mmhhh, smells like bacon",
		"maybe try being less greedy next time?",
		"YOU WERE THE CHOSEN ONE!"
	};

	/**
	 * \brief bind light sources to binding points
	 */
	void fill_lightsources();

	/// @brief compiles all needed shaders for the render loop
	void build_shader_programs();

	/// @brief sets OpenGL states before first draw call, which can't be done in header file
	void prepare_framebuffers();

	/// @brief initializes settings for post processing and rendering
	void set_render_settings() const;
};


