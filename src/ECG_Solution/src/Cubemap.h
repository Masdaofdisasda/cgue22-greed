#pragma once
#include "Utils.h"
#include "gli/gli.hpp"

/// @brief loads and processes 4 different textures for image based lightning
/// loads an equirectangular panorama HDR image and prepares it for the render pipeline
class cubemap{
private:
	GLuint env_id_ = 0; // enviroment created from the hdri image (Cubemap)
	GLuint irrad_id_ = 0; // irradiance (Cubemap)
	GLuint prefilt_id_ = 0; // pre filtered (Cubemap)
	GLuint brdf_lut_id_ = 0; // brdf LUT (Texture)

	void release()
	{
		glDeleteTextures(1, &env_id_);
		glDeleteTextures(1, &irrad_id_);
		glDeleteTextures(1, &prefilt_id_);
		glDeleteTextures(1, &brdf_lut_id_);
		env_id_ = 0;
		irrad_id_ = 0;
		prefilt_id_ = 0;
		brdf_lut_id_ = 0;
	}

	// helper functions only needed for initialization
	void render_cube();
	void render_quad();

public:
	cubemap();
	~cubemap() { release(); }

	/**
	 * \brief load an hdr image from ktx format and generate cubemaps for enviroment mapping, irradiance and pre filtered irradiance
	 * aswell as a 2d texture of the brdf lut
	 * \param tex_path location of the ktx file
	 */
	void load_hdr(const char* tex_path);

	// ensure RAII compliance
	cubemap(const cubemap&) = delete;
	cubemap& operator=(const cubemap&) = delete;

	 cubemap(cubemap&& other) noexcept : env_id_(other.env_id_)
	{
		other.env_id_ = 0; //Use the "null" ID for the old object.
	} 

	cubemap& operator=(cubemap&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			release();
			//obj_ is now 0.
			std::swap(env_id_, other.env_id_);
			std::swap(irrad_id_, other.irrad_id_);
			std::swap(prefilt_id_, other.prefilt_id_);
			std::swap(brdf_lut_id_, other.brdf_lut_id_);
		}
	}

	GLuint get_environment()const { return env_id_; }
	GLuint get_irradiance_id() const { return irrad_id_; }
	GLuint get_pre_filter_id()const { return prefilt_id_; }
	GLuint get_bdrf_lut_id()const { return brdf_lut_id_; }
};
