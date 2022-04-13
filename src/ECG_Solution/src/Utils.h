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
	bool focus_ = true;
	bool cull_ = true;
	bool freezeCull_ = false;
	bool cullDebug_ = false;
	bool debugDrawPhysics_ = false;
	bool usingDebugCamera_ = false;
	float exposure_ = 0.9f;
	float maxWhite_ = 1.07f;
	float bloomStrength_ = 0.2f;
	float adaptationSpeed_ = 0.1f;
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

struct PerFrameData
{
	glm::vec4 viewPos;
	glm::mat4 ViewProj;
	glm::mat4 lavaLevel;
	glm::vec4 bloom;
	glm::vec4 deltaTime;
	glm::vec4 normalMap;
};


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