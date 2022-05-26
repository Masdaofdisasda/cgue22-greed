#pragma once

#include "Program.h"
#include "Texture.h"
#include "Cubemap.h"
#include "LightSource.h"
#include "buffer.h"
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

	std::shared_ptr<global_state> static get_state();
	static std::shared_ptr<global_state> state;

private:
	// Render Settings
	PerFrameData* perframe_data_;
	buffer perframe_buffer_{GL_UNIFORM_BUFFER};

	font_renderer font_renderer_;
	lava_system lava_sim_;

	// Illumination
	light_sources lights_;
	buffer directional_lights_{ GL_UNIFORM_BUFFER }, positional_lights_{ GL_UNIFORM_BUFFER };

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
	// HUD
	program render_image_;		// draw image to full viewport
	program render_color_;		// draw color to full viewport
	program render_waypoint;    // draws marker

	// global Textures
	cubemap ibl_, sky_tex_;
	GLuint lut_3d_ = Texture::load_3dlut("../../assets/textures/look32.CUBE");

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
	GLuint pattern_ = Texture::get_ssao_kernel();

	// Framebuffers for light/shadow
	framebuffer depth_map_fb_ = framebuffer(1024 * state->shadow_res, 1024 * state->shadow_res, 0, GL_DEPTH_COMPONENT24);
	framebuffer blur0_ = framebuffer(state->width / 2, state->height / 2, GL_RGBA16F, 0);
	framebuffer blur1_ = framebuffer(state->width / 2, state->height / 2, GL_RGBA16F, 0);
	//texture from https://github.com/jdupuy/BlueNoiseDitherMaskTiles
	GLuint blue_noise = Texture::load_texture("../../assets/shaders/lightFX/blue_noise_512_512.ktx");
	GLuint perlin_noise = Texture::get_3D_noise(32, 4.0f);

	GLuint way_point = Texture::load_texture("../../assets/shaders/HUD/waypoint.ktx");

	/**
	 * \brief bind light sources to binding points
	 */
	void fill_buffers() const;

	/// @brief compiles all needed shaders for the render loop
	void build_shader_programs();

	/// @brief sets OpenGL states before first draw call, which can't be done in header file
	void prepare_framebuffers();

	/// @brief initializes settings for post processing and rendering
	void set_render_settings() const;

	void draw_hud();
};


