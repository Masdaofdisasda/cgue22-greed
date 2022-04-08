#pragma once
#include "Utils.h"

/// @brief loads and processes 4 different textures for image based lightning
/// loads an equirectangular panorama HDR image and prepares it for the render pipeline
/// for an actual implemention of a cubemap texture use Texture.h
class Cubemap{
private:
	GLuint env_ID = 0; // enviroment created from the hdri image (Cubemap)
	GLuint irrad_ID = 0; // irradiance (Cubemap)
	GLuint prefilt_ID = 0; // pre filtered (Cubemap)
	GLuint brdfLut_ID = 0; // brdf LUT (Texture)

	void Release()
	{
		glDeleteTextures(1, &env_ID);
		glDeleteTextures(1, &irrad_ID);
		glDeleteTextures(1, &prefilt_ID);
		glDeleteTextures(1, &brdfLut_ID);
		env_ID = 0;
		irrad_ID = 0;
		prefilt_ID = 0;
		brdfLut_ID = 0;
	}

	// only needed for initialization
	void renderCube();
	void renderQuad();
	glm::mat4 glmlookAt2(glm::vec3 eye, glm::vec3 target, glm::vec3 up);

public:
	Cubemap();
	~Cubemap() { Release(); }

	void loadHDR(const char* texPath);

	// ensure RAII compliance
	Cubemap(const Cubemap&) = delete;
	Cubemap& operator=(const Cubemap&) = delete;

	 Cubemap(Cubemap&& other) noexcept : env_ID(other.env_ID)
	{
		other.env_ID = 0; //Use the "null" ID for the old object.
	} 

	Cubemap& operator=(Cubemap&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(env_ID, other.env_ID);
			std::swap(irrad_ID, other.irrad_ID);
			std::swap(prefilt_ID, other.prefilt_ID);
			std::swap(brdfLut_ID, other.brdfLut_ID);
		}
	}

	GLuint getEnvironment()const { return env_ID; }
	GLuint getIrradianceID() const { return irrad_ID; }
	GLuint getPreFilterID()const { return prefilt_ID; }
	GLuint getBdrfLutID()const { return brdfLut_ID; }
};
