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
#include "PlayerController.h"
#include "LoadingScreen.h"
#include <irrKlang/irrKlang.h>

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

GlobalState globalState;
KeyboardInputState keyboardInput;
PerFrameData perframeData;
MouseState mouseState;

CameraPositionerInterface* cameraPositioner;
CameraPositioner_FirstPerson floatingPositioner(glm::vec3(0.0f, 1.85f, 70.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
CameraPositioner_Player playerCameraPositioner;
Camera camera(*cameraPositioner);

void registerInputCallbacks(GLFWApp& app);

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

	// setup GLFW window
	printf("Initializing GLFW...");
	GLFWApp GLFWapp(globalState);
	registerInputCallbacks(GLFWapp);

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
	Level level("../../assets/demo.fbx", globalState, perframeData);
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
	btBoxShape* col2 = new btBoxShape(btVector3(500, 0.1, 500));
	physics.createPhysicsObject(
		btVector3(0, 0, 0),
		col2,
		btQuaternion(btVector3(0, 1, 0), btScalar(0)),
		Physics::ObjectMode::Static
	);
	//---------------------------------- /testing ----------------------------------//
	// Setup camera
	cameraPositioner = &playerCameraPositioner;
	camera.setPositioner(cameraPositioner);
	playerCameraPositioner.setPosition(glm::vec3(0, 10, 0));

	// Setup player
	PlayerController player(physics, playerCameraPositioner, glm::vec3(0, 2, 0));
	
	loadingScreen.DrawProgress();

	glm::vec3 lavaPosition = glm::vec3(0.0f, -50.0f, 0.0f); // TODO

	glViewport(0, 0, globalState.width, globalState.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.0f);
	glEnable(GL_CULL_FACE);

	engine->stopAllSounds();
	snd = engine->play2D("../../assets/media/EQ01 Gml Belalua Game 070 Fm.wav", true);

	double timeStamp = glfwGetTime();
	float deltaSeconds = 0.0f;
	FPSCounter fpsCounter = FPSCounter();

	glfwSetInputMode(GLFWapp.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//---------------------------------- RENDER LOOP ----------------------------------//

	printf("Entering render loop...");
	while (!glfwWindowShouldClose(GLFWapp.getWindow()))
	{
		fpsCounter.tick(deltaSeconds);

		// fps counter
		const double newTimeStamp = glfwGetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		std::string title = globalState.window_title + " " + fpsCounter.getFPS() + " fps";
		glfwSetWindowTitle(GLFWapp.getWindow(), title.c_str());

		// variable window size
		glViewport(0, 0, globalState.width, globalState.height);
		GLFWapp.updateWindow();

		// movement
		if (globalState.usingDebugCamera_)
			floatingPositioner.setMovementState(keyboardInput);
		else
			player.move(keyboardInput);

		// calculate physics
		physics.simulateOneStep(deltaSeconds);

		// update camera
		player.updateCameraPositioner();
		cameraPositioner->update(deltaSeconds, mouseState.pos, mouseState.pressedLeft);

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
		if (globalState.debugDrawPhysics_)
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

void registerInputCallbacks(GLFWApp& app) {
	glfwSetKeyCallback(app.getWindow(),
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			// Movement
			const bool press = action != GLFW_RELEASE;
			if (key == GLFW_KEY_W)
				keyboardInput.pressingW = press;
			if (key == GLFW_KEY_S)
				keyboardInput.pressingS = press;
			if (key == GLFW_KEY_A)
				keyboardInput.pressingA = press;
			if (key == GLFW_KEY_D)
				keyboardInput.pressingD = press;
			if (key == GLFW_KEY_1)
				keyboardInput.pressing1 = press;
			if (key == GLFW_KEY_2)
				keyboardInput.pressing2 = press;
			if (mods & GLFW_MOD_SHIFT)
				keyboardInput.pressingShift = press;
			if (key == GLFW_KEY_SPACE)
				keyboardInput.pressingSpace = press;

			// Window management, Debug, Effects
			if (key == GLFW_KEY_ESCAPE)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
			{
				if (globalState.fullscreen_)
					printf("Fullscreen off");
				else
					printf("Fullscreen on");

				globalState.fullscreen_ = !globalState.fullscreen_;
			}
			if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
				globalState.cullDebug_ = !globalState.cullDebug_;
			if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
			{
				if (globalState.bloom_)
					printf("Bloom off");
				else
					printf("Bloom on");

				globalState.bloom_ = !globalState.bloom_;
			}
			if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
			{
				if (globalState.debugDrawPhysics_)
					printf("Physics debugging off");
				else
					printf("Physics debugging on");

				globalState.debugDrawPhysics_ = !globalState.debugDrawPhysics_;
			}
			if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
			{
				if (perframeData.normalMap.x > 0.0f)
					printf("normal mapping off");
				else
					printf("normal mapping on");

				perframeData.normalMap.x *= -1.0f;
			}
			if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
				if (globalState.usingDebugCamera_) {
					printf("Switch camera to player");
					cameraPositioner = &playerCameraPositioner;
					globalState.debugDrawPhysics_ = false;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else {
					printf("Switch camera to debug camera");
					cameraPositioner = &floatingPositioner;
					globalState.debugDrawPhysics_ = true;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				globalState.usingDebugCamera_ = !globalState.usingDebugCamera_;
				camera.setPositioner(cameraPositioner);
			}
			if (key == GLFW_KEY_F7 && action == GLFW_PRESS)
			{
				if (globalState.freezeCull_)
				{
					printf("resume frustum culling\n");
					globalState.freezeCull_ = false;
				}
				else {
					printf("freeze frustum culling\n");
					globalState.freezeCull_ = true;
				}
			}
			if (key == GLFW_KEY_F8 && action == GLFW_PRESS)
			{
				if (globalState.cull_)
				{
					printf("frustum culling off\n");
					globalState.cull_ = false;
				}
				else {
					printf("frustum culling on\n");
					globalState.cull_ = true;
				}
			}
		});
	glfwSetMouseButtonCallback(app.getWindow(),
		[](auto* window, int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				mouseState.pressedLeft = action == GLFW_PRESS;

			if (button == GLFW_MOUSE_BUTTON_RIGHT)
				mouseState.pressedRight = action == GLFW_PRESS;

		});
	glfwSetCursorPosCallback(
		app.getWindow(), [](auto* window, double x, double y) {
			int w, h;
			glfwGetFramebufferSize(window, &w, &h);
			mouseState.pos.x = static_cast<float>(x / w);
			mouseState.pos.y = static_cast<float>(y / h);
		}
	);
}