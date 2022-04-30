/*
* Copyright 2021 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


/*
 Main funtion of the game "Greed" by David Köppl and Nicolas Eder
 contains initialization, resource loading and render loop
*/

#include <sstream>
#include <thread>
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

std::shared_ptr<GlobalState> state;
KeyboardInputState keyboard_input;
PerFrameData perframe_data;
MouseState mouse_state;

camera_positioner_interface* camera_positioner;
camera_positioner_first_person floating_positioner(glm::vec3(0.0f, 1.85f, 70.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
camera_positioner_player player_camera_positioner;
camera camera(*camera_positioner);

void registerInputCallbacks(glfw_app& app);

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

	state = renderer::get_state();

	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	// setup GLFW window
	printf("Initializing GLFW...");
	glfw_app glfw_app(state);
	registerInputCallbacks(glfw_app);

	// load all OpenGL function pointers with GLEW
	printf("Initializing GLEW...\n");
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		EXIT_WITH_ERROR("Failed to load GLEW");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(Debugger::DebugCallbackDefault, 0);

	LoadingScreen loading_screen = LoadingScreen(&glfw_app, state->width, state->height);
	loading_screen.draw_progress();

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

	printf("Initializing scene and render loop...\n");

	printf("Initializing audio...\n");
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	irrklang::ISound* snd = engine->play2D("../../assets/media/EQ07 Prc Fantasy Perc 060.wav", true);

	loading_screen.draw_progress();
	printf("Loading level...\n");
	level level("../../assets/submission1.fbx", state, perframe_data);

	loading_screen.draw_progress();
	printf("Initializing renderer...\n");
	renderer renderer(perframe_data, *level.get_lights());

	loading_screen.draw_progress();
	//Physics Initialization
	printf("Initializing physics...\n");
	Physics physics;

	// Integrate level meshes into physics world
	std::vector<physics_mesh> dynamicMeshes = level.get_dynamic();
	for (auto& dynamicMeshe : dynamicMeshes)
	{
		Physics::PhysicsObject obj = physics.createPhysicsObject(
			dynamicMeshe.node,
			dynamicMeshe.model_trs,
			dynamicMeshe.vtx_positions,
			Physics::ObjectMode::Dynamic
		);
		obj.modelGraphics->game_properties.is_collectable = true; // temporary solution
	}

	std::vector<physics_mesh> staticMeshes = level.get_rigid();
	for (auto& staticMeshe : staticMeshes)
		physics.createPhysicsObject(
			staticMeshe.node,
			staticMeshe.model_trs,
			staticMeshe.vtx_positions,
			Physics::ObjectMode::Static
		);

	// Setup camera
	camera_positioner = &player_camera_positioner;
	camera.set_positioner(camera_positioner);
	player_camera_positioner.set_position(glm::vec3(0, 10, 0));

	PlayerController player(physics, player_camera_positioner, glm::vec3(0, 20, 0));

	auto* item_collection = new ItemCollection();

	auto lava_position = glm::vec3(0.0f, -5.0f, 0.0f);

	glViewport(0, 0, state->width, state->height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.0f);
	glEnable(GL_CULL_FACE);

	engine->stopAllSounds();
	snd = engine->play2D("../../assets/media/Wolum - Greed Collecting.mp3", true);

	double time_stamp = glfwGetTime();
	float delta_seconds = 0.0f;
	fps_counter fps_counter{};

	glfwSetInputMode(glfw_app.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//---------------------------------- RENDER LOOP ----------------------------------//

	printf("Entering render loop...");
	while (!glfwWindowShouldClose(glfw_app.get_window()))
	{
		fps_counter.tick(delta_seconds);

		// fps counter
		delta_seconds = glfw_app.get_delta_seconds();
		std::string title = state->window_title + " " + fps_counter.get_fps() + " fps";
		glfwSetWindowTitle(glfw_app.get_window(), title.c_str());

		// variable window size
		glViewport(0, 0, state->width, state->height);
		glfw_app.update_window();

		// player actions
		if (state->usingDebugCamera_)
			floating_positioner.set_movement_state(keyboard_input);
		else {
			player.move(keyboard_input, delta_seconds);
			state->displayCollectItemHint_ = player.hasCollectableItemInReach();
			player.tryCollectItem(mouse_state, keyboard_input, *item_collection);
		}

		// calculate physics
		physics.simulateOneStep(delta_seconds);

		// update camera
		player.updateCameraPositioner();
		camera_positioner->update(delta_seconds, mouse_state.pos, mouse_state.pressedLeft);

		// calculate and set per frame matrices
		const float ratio = static_cast<float>(state->width) / static_cast<float>(state->height);
		const glm::mat4 projection = glm::perspective(glm::radians(state->fov), ratio, state->Znear, state->Zfar);
		const glm::mat4 view = camera.get_view_matrix();
		perframe_data.ViewProj = projection * view;
		if (perframe_data.deltaTime.y > 60.0f) lava_position.y += delta_seconds * 1.0f; //TODO
		perframe_data.lavaLevel = glm::translate(lava_position);
		perframe_data.viewPos = glm::vec4(camera.get_position(), 1.0f);
		perframe_data.viewInv = glm::inverse(view);
		perframe_data.projInv = glm::inverse(projection);
		perframe_data.deltaTime.x = delta_seconds;
		perframe_data.deltaTime.y += delta_seconds;

		// simple game logic WIP
		state->totalCash = item_collection->getTotalMonetaryValue();
		state->collectedItems = item_collection->size();
		if (perframe_data.viewPos.y > 127.0f)
		{
			state->won_ = true;
		}
		if (perframe_data.viewPos.y < lava_position.y)
		{
			state->lost_ = true;
		}

		// actual draw call
		renderer.draw(&level);
		if (state->debugDrawPhysics_)
			physics.debugDraw();

		// swap buffers
		glfw_app.swap_buffers();
		renderer.swap_luminance();
	}


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	printf("Exiting program...");
	return EXIT_SUCCESS;
}

void registerInputCallbacks(glfw_app& app) {
	glfwSetKeyCallback(app.get_window(),
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			// Movement
			const bool press = action != GLFW_RELEASE;
			if (key == GLFW_KEY_W)
				keyboard_input.pressingW = press;
			if (key == GLFW_KEY_S)
				keyboard_input.pressingS = press;
			if (key == GLFW_KEY_A)
				keyboard_input.pressingA = press;
			if (key == GLFW_KEY_D)
				keyboard_input.pressingD = press;
			if (key == GLFW_KEY_1)
				keyboard_input.pressing1 = press;
			if (key == GLFW_KEY_2)
				keyboard_input.pressing2 = press;
			if (mods & GLFW_MOD_SHIFT)
				keyboard_input.pressingShift = press;
			if (key == GLFW_KEY_SPACE)
				keyboard_input.pressingSpace = press;

			// Window management, Debug, Effects
			if (key == GLFW_KEY_ESCAPE)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
			{
				if (state->fullscreen_)
					printf("Fullscreen off\n");
				else
					printf("Fullscreen on\n");

				state->fullscreen_ = !state->fullscreen_;
			}
			if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
				state->cullDebug_ = !state->cullDebug_;
			if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
			{
				if (state->bloom_)
					printf("Bloom off\n");
				else
					printf("Bloom on\n");

				state->bloom_ = !state->bloom_;
			}
			if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
			{
				if (state->debugDrawPhysics_)
					printf("Physics debugging off");
				else
					printf("Physics debugging on");

				state->debugDrawPhysics_ = !state->debugDrawPhysics_;
			}
			if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
			{
				if (perframe_data.normalMap.x > 0.0f)
					printf("normal mapping off");
				else
					printf("normal mapping on");

				perframe_data.normalMap.x *= -1.0f;
			}
			if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
				if (state->usingDebugCamera_) {
					printf("Switch camera to player");
					camera_positioner = &player_camera_positioner;
					state->debugDrawPhysics_ = false;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else {
					printf("Switch camera to debug camera");
					camera_positioner = &floating_positioner;
					state->debugDrawPhysics_ = true;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				state->usingDebugCamera_ = !state->usingDebugCamera_;
				camera.set_positioner(camera_positioner);
			}
			if (key == GLFW_KEY_F7 && action == GLFW_PRESS)
			{
				if (state->freezeCull_)
				{
					printf("resume frustum culling\n");
					state->freezeCull_ = false;
				}
				else {
					printf("freeze frustum culling\n");
					state->freezeCull_ = true;
				}
			}
			if (key == GLFW_KEY_F8 && action == GLFW_PRESS)
			{
				if (state->cull_)
				{
					printf("frustum culling off\n");
					state->cull_ = false;
				}
				else {
					printf("frustum culling on\n");
					state->cull_ = true;
				}
			}
			if (key == GLFW_KEY_F9 && action == GLFW_PRESS)
			{
				if (state->ssao_)
				{
					printf("SSAO off\n");
					state->ssao_ = false;
				}
				else {
					printf("SSAO on\n");
					state->ssao_ = true;
				}
			}
		});
	glfwSetMouseButtonCallback(app.get_window(),
		[](auto* window, int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				mouse_state.pressedLeft = action == GLFW_PRESS;

			if (button == GLFW_MOUSE_BUTTON_RIGHT)
				mouse_state.pressedRight = action == GLFW_PRESS;

		});
	glfwSetCursorPosCallback(
		app.get_window(), [](auto* window, double x, double y) {
			int w, h;
			glfwGetFramebufferSize(window, &w, &h);
			mouse_state.pos.x = static_cast<float>(x / w);
			mouse_state.pos.y = static_cast<float>(y / h);
		}
	);
}