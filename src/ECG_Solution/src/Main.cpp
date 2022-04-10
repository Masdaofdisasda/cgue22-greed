/*
* Copyright 2021 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


/*
 Main funtion of the game "Greed" by David K�ppl and Nicolas Eder
 contains initialization, resource loading and render loop
*/

#pragma once
#include <sstream>
#include <numbers>
#include "Camera.h"
#include "Renderer.h"
#include "FPSCounter.h"
#include "GLFWApp.h"
#include "Debugger.h"
#include "Level.h"
#include "Physics.h"
#include "LoadingScreen.h"
#include <irrKlang/irrKlang.h>

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

GlobalState globalState;

PerFrameData perframeData;

struct MouseState
{
	glm::vec2 pos = glm::vec2(0.0f);
	bool pressedLeft = false;
	bool pressedRight = false;
} mouseState;


CameraPositioner_FirstPerson positioner(glm::vec3(0.0f, 1.85f, 70.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
Camera camera(positioner);

//static btConvexHullShape* getHullShapeFromMesh(Mesh* mesh);
static btRigidBody makeRigidbody(btQuaternion rot, btVector3 pos, btCollisionShape* col, btScalar mass);
static glm::vec3 btToGlmVector(btVector3 input);

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	printf("Starting program...\n");

	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	std::ifstream file("../../assets/demo.fbx");
	// if this assertion fails, and you cloned this project from Github,
	// try setting your working directory of the debugger to "$(TargetDir)"
	assert(file.is_open());
	file.close();

	globalState = Renderer::loadSettings();

	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	//setup GLFW window
	GLFWApp GLFWapp(globalState);

	// register input callbacks to window
	glfwSetKeyCallback(GLFWapp.getWindow(),
		[](GLFWwindow* window,
			int key, int scancode, int action, int mods)
		{
			// Movement
			const bool press = action != GLFW_RELEASE;
			if (key == GLFW_KEY_ESCAPE)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			if (key == GLFW_KEY_W)
				positioner.movement_.forward_ = press;
			if (key == GLFW_KEY_S)
				positioner.movement_.backward_ = press;
			if (key == GLFW_KEY_A)
				positioner.movement_.left_ = press;
			if (key == GLFW_KEY_D)
				positioner.movement_.right_ = press;
			if (key == GLFW_KEY_1)
				positioner.movement_.up_ = press;
			if (key == GLFW_KEY_2)
				positioner.movement_.down_ = press;
			if (mods & GLFW_MOD_SHIFT)
				positioner.movement_.fastSpeed_ = press;
			if (key == GLFW_KEY_SPACE)
				positioner.setUpVector(glm::vec3(0.0f, 1.0f, 0.0f));

			// Debug & Effects
			if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
			{
				if (globalState.fullscreen_)
				{
					printf("Fullscreen off");
					globalState.fullscreen_ = false;
				}
				else {
					printf("Fullscreen on");
					globalState.fullscreen_ = true;
				}
			}
			if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
			{
				if (globalState.cullDebug_)
				{
					globalState.cullDebug_ = false;
				}
				else
				{
					globalState.cullDebug_ = true;
				}
			}
			if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
			{
				if (globalState.bloom_)
				{
					printf("Bloom off");
					globalState.bloom_ = false;
				}
				else {
					printf("Bloom on");
					globalState.bloom_ = true;
				}
			}
			if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
			{
				if (globalState.debugDrawPhysics)
				{
					printf("Physics debugging off");
					globalState.debugDrawPhysics = false;
				}
				else {
					printf("Physics debugging on");
					globalState.debugDrawPhysics = true;
				}
			}
			if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
			{
				if (perframeData.normalMap.x > 0.0f)
				{
					printf("normal mapping off");
					perframeData.normalMap.x *= -1.0f;
				}
				else {
					printf("normal mapping on");
					perframeData.normalMap.x *= -1.0f;
				}
			}
		});
	glfwSetMouseButtonCallback(GLFWapp.getWindow(),
		[](auto* window, int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				mouseState.pressedLeft = action == GLFW_PRESS;

			if (button == GLFW_MOUSE_BUTTON_RIGHT)
				mouseState.pressedRight = action == GLFW_PRESS;

		});
	glfwSetCursorPosCallback(
		GLFWapp.getWindow(), [](auto* window, double x, double y) {
			int w, h;
			glfwGetFramebufferSize(window, &w, &h);
			mouseState.pos.x = static_cast<float>(x / w);
			mouseState.pos.y = static_cast<float>(y / h);
			//glfwSetCursorPos(window, 0, 0); // cursor disabled kind of fix
		}
	);

	// load all OpenGL function pointers with GLEW
	printf("Initializing GLEW...\n");
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		EXIT_WITH_ERROR("Failed to load GLEW");

	//part of the ECG magical framework
	printf("Initializing framework...\n");
	if (!initFramework())
		EXIT_WITH_ERROR("Failed to init framework");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(Debugger::DebugCallbackDefault, 0);

	LoadingScreen loadingScreen(&GLFWapp, globalState.width, globalState.height);
	loadingScreen.DrawProgress();

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

	printf("Initializing scene and render loop...\n");

	printf("Intializing audio...\n"); 
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	irrklang::ISound* snd = engine->play2D("../../assets/media/EQ07 Prc Fantasy Perc 060.wav", true);

	printf("Loading level...\n");
	Level level("../../assets/demo.fbx", globalState);
	loadingScreen.DrawProgress();

	printf("Intializing renderer...\n");
	Renderer renderer(globalState, perframeData, *level.getLights());
	loadingScreen.DrawProgress();

	//Physics Initialization
	printf("Initializing physics...\n");
	Physics physics;
	//---------------------------------- testing ----------------------------------//
	btBoxShape* col = new btBoxShape(btVector3(0.5, 0.5, 0.5));
	physics.createPhysicsObject(
		btVector3(0, 5, 0),
		col,
		btQuaternion(btVector3(1, 0, 0), btScalar(45)),
		Physics::ObjectMode::Dynamic
	);
	btBoxShape* col2 = new btBoxShape(btVector3(5, 0.1, 5));
	physics.createPhysicsObject(
		btVector3(0, -5, 0),
		col2,
		btQuaternion(btVector3(0, 1, 0), btScalar(0)),
		Physics::ObjectMode::Static
	);
	//---------------------------------- /testing ----------------------------------//
	loadingScreen.DrawProgress();

	glm::vec3 lavaPosition = glm::vec3(0.0f, -50.0f, 0.0f); // TODO

	glViewport(0, 0, globalState.width, globalState.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_CULL_FACE);

	engine->stopAllSounds();
	snd = engine->play2D("../../assets/media/EQ01 Gml Belalua Game 070 Fm.wav", true);

	double timeStamp = glfwGetTime();
	float deltaSeconds = 0.0f;
	FPSCounter fpsCounter = FPSCounter();

	glfwSetInputMode(GLFWapp.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//---------------------------------- RENDER LOOP ----------------------------------//

	printf("Entering render loop...");
	while (!glfwWindowShouldClose(GLFWapp.getWindow()))
	{
		fpsCounter.tick(deltaSeconds);

		//positioner.update(deltaSeconds, mouseState.pos, globalState.focus_);
		positioner.update(deltaSeconds, mouseState.pos, mouseState.pressedLeft);

		// fps counter
		const double newTimeStamp = glfwGetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		std::string title = globalState.window_title + " " + fpsCounter.getFPS() + " fps";
		glfwSetWindowTitle(GLFWapp.getWindow(), title.c_str());

		// variable window size
		glViewport(0, 0, globalState.width, globalState.height);
		GLFWapp.updateWindow();

		// calculate physics
		physics.simulateOneStep(deltaSeconds);

		// calculate and set per Frame matrices
		const float ratio = globalState.width / (float)globalState.height;
		const glm::mat4 projection = glm::perspective(glm::radians(globalState.fov), ratio, globalState.Znear, globalState.Zfar);
		const glm::mat4 view = camera.getViewMatrix();
		perframeData.ViewProj = projection * view;
		lavaPosition.y += deltaSeconds * 1.0f;
		perframeData.lavaLevel = glm::translate(lavaPosition);
		perframeData.viewPos = glm::vec4(camera.getPosition(), 1.0f);
		perframeData.deltaTime.x = deltaSeconds;

		// actual draw call
		renderer.Draw(&level);
		if (globalState.debugDrawPhysics)
			physics.debugDraw();

		// swap buffers
		GLFWapp.swapBuffers();
		renderer.swapLuminance();
	}

	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */


	printf("Exiting programm...");
	return EXIT_SUCCESS;
}

