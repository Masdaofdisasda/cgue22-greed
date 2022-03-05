/*
* Copyright 2021 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


#pragma once
#include <sstream>
#include "Program.h"
#include "Texture.h"
#include "Camera.h"
#include "UBO.h"

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

// used for activating camera tranformation
bool useBall = false;
bool useZoom = false;
bool useStrafe = false;
// used for zooming by scrolling
float yOffset = 0.0f;
// used for calculating mouse cursor deltas
glm::vec3 prev = glm::vec3(0, 0, 0);
// used for toggling render settings
bool useCulling = true;
bool useWireFrame = false;
bool useRotatingObject = false;
// used for fps count
double prevTime = 0.0;
double curTime = 0.0;
double deltaTime;
unsigned int tCount = 0;
// used for animations
float s = 0.0f;

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	std::cout << "starting program..." << std::endl;
	std::cout << std::endl;

	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	// init reader for ini files
	std::cout << "reading setting from settings.ini..." << std::endl;
	INIReader reader("assets/settings.ini");
	// load values from ini file
	// first param: section [window], second param: property name, third param: default value
	const int width = reader.GetInteger("window", "width", 800);
	const int height = reader.GetInteger("window", "height", 800);
	const int refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	const bool fullscreen = reader.GetBoolean("window", "fullscreen", false);
	std::string window_title = reader.Get("window", "title", "ECG 2021");
	const float fov = reader.GetReal("camera", "fov", 60.0f);
	const float Znear = reader.GetReal("camera", "near", 0.1f);
	const float Zfar = reader.GetReal("camera", "far", 100.0f);
	std::cout << "settings loaded:" << std::endl
		<< "width = " << width << std::endl
		<< "height = " << height << std::endl
		<< "refresh rate = " << refresh_rate << std::endl
		<< "fullscreen = " << fullscreen << std::endl
		<< "window title = " << window_title << std::endl
		<< "field of view = " << fov << std::endl
		<< "near value = " << Znear << std::endl
		<< "far value = " << Zfar << std::endl;
	std::cout << std::endl;

	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	
	// load GLFW 
	// false if something goes wrong
	if (!glfwInit())
	{
		EXIT_WITH_ERROR("Failed to init GLFW");
	}

	// GLFW should use OpenGL Version 4.6 with core functions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); 
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwSwapInterval(0);

	// creates new GLFWwindow Object using data from settings.ini
	std::cout << "create GLFWwindow Object..." << std::endl;
	GLFWwindow* window = glfwCreateWindow(width, height, window_title.c_str(),  nullptr, nullptr);
	// check if glfwCreateWindow was succesful
	if (window == nullptr)
	{
		EXIT_WITH_ERROR("failed to create GLFW window");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// introduce window into context
	glfwMakeContextCurrent(window);

	// register input callbacks to window
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// load all OpenGL function pointers with GLEW
	std::cout << "initializing GLEW..." << std::endl;
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		EXIT_WITH_ERROR("failed to load GLEW");
	}

	//do not delete this
	std::cout << "initialize framework..." << std::endl;
	if (!initFramework())
	{
		EXIT_WITH_ERROR("failed to init framework");
	}
	std::cout << std::endl;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallbackDefault, 0);

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

	
	// directional light
	std::vector <DirectionalLight> dLightsBuffer;
	dLightsBuffer.push_back(DirectionalLight{
		glm::vec4(0.0f, -1.0f, -1.0f ,1.0f),		// direction

		glm::vec4(0.8f, 0.8f, 0.8f ,1.0f), });		// intensity 

	// positional light
	std::vector <PositionalLight> pLightsBuffer;
	pLightsBuffer.push_back(PositionalLight{
		glm::vec4(0.0f,  0.0f,  0.0f ,1.0f),		// position
		glm::vec4(1.0f, 0.4f, 0.1f, 1.0f),			// attenuation (constant, linear, quadratic)

		glm::vec4(1.0f, 1.0f, 1.0f ,1.0f) });		// intensity
	

	// spot light
	std::vector <SpotLight> sLightsBuffer;
	sLightsBuffer.push_back(SpotLight{
		glm::vec4(5.0f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(7.0f)),glm::cos(glm::radians(5.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

	sLightsBuffer.push_back(SpotLight{
		glm::vec4(2.5f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(14.0f)),glm::cos(glm::radians(10.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

	sLightsBuffer.push_back(SpotLight{
		glm::vec4(0.0f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(28.0f)),glm::cos(glm::radians(20.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

	sLightsBuffer.push_back(SpotLight{
		glm::vec4(-2.5f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(56.0f)),glm::cos(glm::radians(40.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity
	sLightsBuffer.push_back(SpotLight{
		glm::vec4(-5.0f,-4.0f,-2.0f,1.0f), // position
		glm::vec4(0.0f,-9.0f,-1.0f,1.0f), // direction
		glm::vec4(glm::cos(glm::radians(112.0f)),glm::cos(glm::radians(80.0f)),1.0f,1.0f), // angles (outer, inner)
		glm::vec4(1.0f,0.09f,0.032f,1.0f), // attenuation (constant, linear, quadratic)

		glm::vec4(0.8f,0.8f,0.8f,1.0f) }); // intensity

	Texture brickDiff("assets/textures/brick03-diff.jpeg");
	Texture brickSpec("assets/textures/brick03-spec.jpeg");
	Cubemap brickCube("assets/textures/cubemap");

	Material brick(&brickDiff,&brickSpec,&brickCube,
		glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), 1.0f);

	Material sky(&brickDiff, &brickSpec, &brickCube,
		glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);

	Mesh skybox = skybox.Skybox(40.0f, &sky);
	skybox.translate(glm::vec3(0.0f, -5.0f, 0.0f));

	Mesh box = box.Cube(1.5f, 1.5f, 1.5f, &brick);
	box.translate(glm::vec3(0.0f, -2.0f, 0.0f));


	// build shader programms
	Shader pbrVert("assets/shaders/pbr/pbr.vert");
	Shader pbrFrag("assets/shaders/pbr/pbr.frag", glm::ivec3(dLightsBuffer.size(), pLightsBuffer.size(), sLightsBuffer.size()));
	Program PBRShader(pbrVert, pbrFrag);
	PBRShader.Use();


	// create Uniform Buffer Objects from light source struct vectors
	UBO directionalLights = UBO(dLightsBuffer);
	UBO positionalLights = UBO(pLightsBuffer);
	UBO spotLights = UBO(sLightsBuffer);

	// bind UBOs to bindings in shader
	PBRShader.bindLightBuffers(&directionalLights, &positionalLights, &spotLights);
	// set light source count variables
	PBRShader.setuInt("dLightCount", dLightsBuffer.size());
	PBRShader.setuInt("pLightCount", pLightsBuffer.size());
	PBRShader.setuInt("sLightCount", sLightsBuffer.size());



	// Use Depth Buffer
	std::cout << "enable depth buffer..." << std::endl;
	glEnable(GL_DEPTH_TEST);

	std::cout << "create Camera..." << std::endl;
	glm::vec3 pos = glm::vec3(0.0f,0.0f, 6.0f);
	glm::vec3 front = glm::vec3(0.0f,0.0f, -6.0f);
	glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
	Camera camera(pos, front, up, fov, width, height, Znear, Zfar);

	//---------------------------------- RENDER LOOP ----------------------------------//
	std::cout << "enter render loop..." << std::endl << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		// fps counter
		curTime = glfwGetTime();
		deltaTime = curTime - prevTime;
		tCount++;
		if (deltaTime >= 1.0/30.0)
		{
			std::string fps = std::to_string((1.0 / deltaTime) * tCount);
			std::string title = window_title + " " + fps + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		// animate objects
		if (useRotatingObject)
		{
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), s, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 traBox = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f));
			box.model = traBox * rot;
			s += 0.01f;
		}

		// toggle wireframe mode with F1 key
		glPolygonMode(GL_FRONT_AND_BACK, useWireFrame ? GL_LINE : GL_FILL);
		useCulling ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

		// prepare depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //RGBA

		// handle input
		glViewport(0, 0, width, height);
		glfwPollEvents();
		if (useBall) camera.BallArc(prev, window);
		if (useZoom){ camera.Zoom(yOffset); useZoom = false; } // prevents infinity zooms}
		if (useStrafe) { camera.Strafe(prev, window); }

		PBRShader.Use();

		// update camera for shader
		PBRShader.setVec3("viewPos", camera.camPos);
		PBRShader.setMat4("viewProject", camera.getViewProj());

		// draw phong shaded meshes
		PBRShader.Draw(box);


		// draw skybox        
		glDepthFunc(GL_LEQUAL);
		PBRShader.setMat4("viewProject", camera.getViewProjSkybox());
		PBRShader.Draw(skybox);
		glDepthFunc(GL_LESS);


		// swap back and front buffers
		glfwSwapBuffers(window);
	}

	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();
	glfwDestroyWindow(window);
	glfwTerminate();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */


	std::cout << "exit programm..." << std::endl;

	return EXIT_SUCCESS;
}


// callback prototype
// key: pressed key, compare GLFW_KEY_*
// action: GLFW_PRESS or GLFW_RELEASE
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// close window when ESC key is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		std::cout << "register ESC key press..." << std::endl;
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		std::cout << "register F1 key press..." << std::endl;

		// toggle between solid and wireframe rendering by pressing F1
		if (useWireFrame)
		{
			std::cout << "disabling wire-frame mode..." << std::endl;
			useWireFrame = false;
		}
		else {
			std::cout << "enabling wire-frame mode..." << std::endl;
			useWireFrame = true;
		}
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		std::cout << "register F2 key press..." << std::endl;

		// toggle between using culling or not by pressing F2
		if (useCulling)
		{
			std::cout << "disabling back-face culling..." << std::endl;
			useCulling = false;
		}
		else {
			std::cout << "enabling back-face culling..." << std::endl;
			useCulling = true;
		}
	}

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		std::cout << "register F3 key press..." << std::endl;

		// toggle between using culling or not by pressing F2
		if (!useRotatingObject)
		{
			std::cout << "rotate objects..." << std::endl;
			useRotatingObject = true;
		}
		else {
			std::cout << "stop rotation..." << std::endl;
			useRotatingObject = false;
		}
	}

}

void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	// register left mouse button press
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// save mouse cursor location for calulation
		double xCur = 0, yCur = 0;
		glfwGetCursorPos(window, &xCur, &yCur);
		prev = glm::vec3(xCur, yCur, -1);

		// active use of ballarc camera
		useBall = true;
	}

	// register left mouse button release
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// deactivate ballarc camera
		useBall = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		// save mouse cursor location for calulation
		double xCur = 0, yCur = 0;
		glfwGetCursorPos(window, &xCur, &yCur);
		prev = glm::vec3(xCur, yCur, -1);

		// activate strafe camera
		useStrafe = true;
	}

	// register left mouse button release
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		// deactivate ballarc camera
		useStrafe = false;
	}

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset != 0)
	{
		useZoom = true;
		yOffset = yoffset;
	}
}


static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from nvidia
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}

