#pragma once
#include "Utils.h"

class Cubemap
{
private:
	GLuint hdr_ID = 0; // rectangular image in hdri format (Texture)
	GLuint env_ID = 0; // enviroment (Cubemap)
	GLuint irrad_ID = 0; // irradiance (Cubemap)
	GLuint prefilt_ID = 0; // pre filtered (Cubemap)
	GLuint brdfLut_ID = 0; // brdf LUT (Texture)

	string path = "";

	void Release()
	{
		glDeleteTextures(1, &hdr_ID);
		glDeleteTextures(1, &env_ID);
		glDeleteTextures(1, &irrad_ID);
		glDeleteTextures(1, &prefilt_ID);
		glDeleteTextures(1, &brdfLut_ID);
		hdr_ID = 0;
		env_ID = 0;
		irrad_ID = 0;
		prefilt_ID = 0;
		brdfLut_ID = 0;
	}

	void renderCube();
	void renderQuad();
	glm::mat4 glmlookAt2(glm::vec3 eye, glm::vec3 target, glm::vec3 up);

public:
	Cubemap();
	~Cubemap() { Release(); }

	void loadHDR(const char* texPath);
	bool equals(string tex);

	// ensure RAII compliance
	Cubemap(const Cubemap&) = delete;
	Cubemap& operator=(const Cubemap&) = delete;

	 Cubemap(Cubemap&& other) noexcept : hdr_ID(other.hdr_ID)
	{
		other.hdr_ID = 0; //Use the "null" ID for the old object.
	} 

	Cubemap& operator=(Cubemap&& other)
	{
		//ALWAYS check for self-assignment.
		if (this != &other)
		{
			Release();
			//obj_ is now 0.
			std::swap(hdr_ID, other.hdr_ID);
			std::swap(env_ID, other.env_ID);
			std::swap(irrad_ID, other.irrad_ID);
			std::swap(prefilt_ID, other.prefilt_ID);
			std::swap(brdfLut_ID, other.brdfLut_ID);
		}
	}

	GLuint getHdr() const { return hdr_ID; }
	GLuint getEnvironment()const { return env_ID; }
	GLuint getIrradianceID() const { return irrad_ID; }
	GLuint getPreFilterID()const { return prefilt_ID; }
	GLuint getBdrfLutID()const { return brdfLut_ID; }

	string getPath() { return path; }
};
