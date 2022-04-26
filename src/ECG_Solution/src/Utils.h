/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include "INIReader.h"
#include <iostream>
#include <Windows.h>
#include <memory>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#ifndef _GLOBAL_STRUCTS_
#define _GLOBAL_STRUCTS_
struct GlobalState
{
	int width = 800;
	int height = 800;
	int refresh_rate = 60;
	bool fullscreen_ = false;
	std::string window_title = "Greed";
	float fov = 60;
	float Znear = 0.1;
	float Zfar = 1000.0;

	bool bloom_ = true;
	bool cull_ = true;
	bool freezeCull_ = false;
	bool ssao_ = true;
	bool cullDebug_ = false;
	bool debugDrawPhysics_ = false;
	bool usingDebugCamera_ = false;
	//bloom
	float exposure_ = 0.9f;
	float maxWhite_ = 1.07f;
	float bloomStrength_ = 0.2f;
	float adaptationSpeed_ = 0.1f;
	//ssao
	float scale_ = 1.0f;
	float bias_ = 0.2f;
	float radius = 0.2f;
	float attScale = 1.0f;
	float distScale = 0.5f;
	//lightFX
	int shadowRes_ = 4;
	//game logic
	bool won_ = false;
	bool lost_ = false;
	//ui
	bool displayCollectItemHint_ = false;
};

struct KeyboardInputState {
	bool pressingW = false;
	bool pressingS = false;
	bool pressingA = false;
	bool pressingD = false;
	bool pressing1 = false;
	bool pressing2 = false;
	bool pressingShift = false;
	bool pressingSpace = false;
	bool pressingE = false;
	bool pressingQ = false;
};

struct MouseState
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressedLeft = false;
	bool pressedRight = false;
};

/*files using this structure:
Frustumviewer.vert
AABBviwer.vert
bulletDebug.vert
combineHDR.frag
lavafloor.vert
lightadaption.comp
pbr.vert/.frag
skybox.vert/.frag
SSAO.frag
combineSSAO.frag
depthMap.frag/.vert
VolumetricLight.frag
*/
struct PerFrameData
{
	glm::vec4 viewPos;		// view/eye vector
	glm::mat4 ViewProj;		// view projection matrix
	glm::mat4 lavaLevel;	// lava translation matrix
	glm::mat4 lightViewProj;// light matrix for shadowmapping
	glm::mat4 viewInv;
	glm::mat4 projInv;
	glm::vec4 bloom;		// x = exposure, y = maxWhite, z = bloomStrength, w = adaptionSpeed
	glm::vec4 deltaTime;	// x = deltaSeconds, y = summedTime, z = ?, w = ?
	glm::vec4 normalMap;	// x = normalMapToogle, y = ?, z = ?, w = ?
	glm::vec4 ssao1;		// x = scale,, y = bias, z = znear, w = zfar
	glm::vec4 ssao2;		// x = radius, y = attscale, z = distscale, w = ?
};
#endif

#define EXIT_WITH_ERROR(err) \
        { \
                glfwTerminate(); \
                std::cout << "ERROR: " << err << std::endl; \
                system("PAUSE"); \
                return EXIT_FAILURE; \
        }


#define FOURCC_DXT1	MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT3	MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT5	MAKEFOURCC('D', 'X', 'T', '5')

/*!
 * A loaded '.dss' image
 */
class DDSImage {
public:
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	GLenum format;

	DDSImage() : data(nullptr), width(0), height(0), size(0), format(GL_NONE) {}
	DDSImage(const DDSImage& img) = delete;
	DDSImage(DDSImage&& img) : data(img.data), width(img.width), height(img.height), size(img.size), format(img.format) {
		img.data = nullptr;
	}
	DDSImage& operator=(const DDSImage& img) = delete;
	DDSImage& operator=(DDSImage&& img) {
		data = img.data;
		img.data = nullptr;
		width = img.width;
		height = img.height;
		size = img.size;
		format = img.format;
		return *this;
	};

	~DDSImage() { if (data != nullptr) { delete[] data; data = nullptr; } }
};


/* --------------------------------------------- */
// Framework functions
/* --------------------------------------------- */

/*!
 * Initializes the framework
 * Do not overwrite this function!
 */
bool initFramework();

/*!
 * Draws a teapot
 */
void drawTeapot();

/*!
 * Destroys the framework
 * Do not overwrite this function!
 */
void destroyFramework();

/*!
 * Loads a '.dss' image from a file
 * @param file: the path to the image file
 * @return a loaded DSS image
 */
DDSImage loadDDS(const char* file);

glm::vec3 translationFromTransform(glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return translation;
}

glm::quat rotationFromTransform(glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return rotation;
}

glm::vec3 scaleFromTransform(glm::mat4 transform) {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	return scale;
}