/*
* Copyright 2021 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


/*
 Main funtion of the game "Greed" by David Köppl and Nicolas Eder
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
#include "BulletDebugDrawer.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

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


CameraPositioner_FirstPerson positioner(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
Camera camera(positioner);
const double PI = 3.141592653589793238463;

static btConvexHullShape* getHullShapeFromMesh(Mesh* mesh);
static btRigidBody makeRigidbody(btQuaternion rot, btVector3 pos, btCollisionShape* col, btScalar mass);
static void print(string s);
static glm::vec3 btToGlmVector(btVector3 input);

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	print("starting program...\n");

	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	globalState = Renderer::loadSettings(globalState);

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
					print("fullscreen off");
					globalState.fullscreen_ = false;
				}
				else {
					print("fullscreen on");
					globalState.fullscreen_ = true;
				}
			}
			if (key == GLFW_KEY_F2 && action == GLFW_PRESS) //TODO
			{
				if (globalState.focus_)
				{
					globalState.request_focus_ = true;
				}
				else
				{

					globalState.request_unfocus_ = true;
				}
			}
			if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
			{
				if (globalState.bloom_)
				{
					print("bloom off");
					globalState.bloom_ = false;
				}
				else {
					print("bloom on");
					globalState.bloom_ = true;
				}
			}
			if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
			{
				if (globalState.debugDrawPhysics)
				{
					print("physics debugging off");
					globalState.debugDrawPhysics = false;
				}
				else {
					print("physics debugging on");
					globalState.debugDrawPhysics = true;
				}
			}
			if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
			{
				if (perframeData.normalMap.x > 0.0f)
				{
					print("normal mapping off");
					perframeData.normalMap.x *= -1.0f;
				}
				else {
					print("normal mapping on");
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
	print("initializing GLEW...");
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		EXIT_WITH_ERROR("failed to load GLEW");

	//part of the ECG magical framework
	print("initialize framework...\n");
	if (!initFramework())
		EXIT_WITH_ERROR("failed to init framework");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(Debugger::DebugCallbackDefault, 0);

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

	print("initialize scene and render loop...");

	// load models and textures
	print("loading level...");
	//Level level("assets/Bistro_v5_2/BistroInterior.fbx"); // https://developer.nvidia.com/orca/amazon-lumberyard-bistro
	Level level("assets/test.fbx"); 
	print("intializing renderer...");
	Renderer renderer(globalState, perframeData, *level.getLights());


	//Bullet Initialization
	printf("Initializing bullet physics...\n");
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collision_configuration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collision_configuration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));
	BulletDebugDrawer* bulletDebugDrawer = new BulletDebugDrawer();
	bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamics_world->setDebugDrawer(bulletDebugDrawer);

	//btCollisionShape* collider = getHullShapeFromMesh(&coin1);
	btVector3* boxSize = new btVector3(1.0, 0.25, 1.0);
	btCollisionShape* collider = new btBoxShape(*boxSize);
	btRigidBody fallingCoin = makeRigidbody(btQuaternion(btVector3(1, 0, 0), 45), btVector3(0.0, 0.0, 0.0), collider, 1);

	btVector3* boxSize2 = new btVector3(20, 0.0, 20.0);
	btCollisionShape* collider2 = new btBoxShape(*boxSize2);
	btRigidBody staticPlane = makeRigidbody(btQuaternion(0.0, 0.0, 0.0), btVector3(0.0, -10.0, 0.0), collider2, 0);

	dynamics_world->addRigidBody(&fallingCoin);
	dynamics_world->addRigidBody(&staticPlane);
	//-------------------------/WIP------------------------------------------//

	glViewport(0, 0, globalState.width, globalState.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	double timeStamp = glfwGetTime();
	float deltaSeconds = 0.0f;
	FPSCounter fpsCounter = FPSCounter();

	glfwSetInputMode(GLFWapp.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//---------------------------------- RENDER LOOP ----------------------------------//

	print("enter render loop...");
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
		/*
		dynamics_world->stepSimulation(deltaSeconds);

		glm::vec3 pos = btToGlmVector(fallingCoin.getCenterOfMassTransform().getOrigin());
		float deg = (float)(fallingCoin.getOrientation().getAngle() * 180 / PI);
		glm::vec3 axis = btToGlmVector(fallingCoin.getOrientation().getAxis());
		glm::vec3 scale = glm::vec3(0.5);
		models[0]->setMatrix(pos, deg, axis, scale);

		glm::vec3 pos2 = btToGlmVector(staticPlane.getCenterOfMassTransform().getOrigin());
		float deg2 = (float)(staticPlane.getOrientation().getAngle() * 180 / PI);
		glm::vec3 axis2 = btToGlmVector(staticPlane.getOrientation().getAxis());
		glm::vec3 scale2 = glm::vec3(20.0f, 1.0f, 20.0f);
		models[3]->setMatrix(pos2, deg2, axis2, scale2);*/

		// calculate and set per Frame matrices
		const float ratio = globalState.width / (float)globalState.height;
		const glm::mat4 projection = glm::perspective(glm::radians(globalState.fov), ratio, globalState.Znear, globalState.Zfar);
		const glm::mat4 view = camera.getViewMatrix();
		perframeData.ViewProj = projection * view;
		perframeData.ViewProjSkybox = projection * glm::mat4(glm::mat3(view)); // remove translation
		perframeData.viewPos = glm::vec4(camera.getPosition(), 1.0f);

		// actual draw call
		renderer.Draw(&level);
		if (globalState.debugDrawPhysics) {
			dynamics_world->debugDrawWorld();
			bulletDebugDrawer->draw();
		}

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


	print("exit programm...");

	return EXIT_SUCCESS;
}

static btConvexHullShape* getHullShapeFromMesh(Mesh* mesh) {
	btConvexHullShape* shape = new btConvexHullShape();
	btScalar* coordinates = (*mesh).getVerticeCoordinates();
	int verticeAmount = (*mesh).getVerticeAmount();
	for (int i = 0; i < verticeAmount; i++)
	{
		btScalar x = coordinates[i * 3];
		btScalar y = coordinates[i * 3 + 1];
		btScalar z = coordinates[i * 3 + 2];
		shape->addPoint(btVector3(x, y, z));
	}

	return shape;
}

static btRigidBody makeRigidbody(btQuaternion rot, btVector3 pos, btCollisionShape* col, btScalar mass) {

	btTransform* startTransform = new btTransform(rot, pos);
	btMotionState* motionSate = new btDefaultMotionState(*startTransform);
	btVector3 inertia;
	col->calculateLocalInertia(mass, inertia);
	return btRigidBody(mass, motionSate, col, inertia);
}

static void print(string s) {
	std::cout << s << std::endl;
}

static glm::vec3 btToGlmVector(btVector3 input) {
	return glm::vec3((float)input.getX(), (float)input.getY(), (float)input.getZ());
}