#pragma once
#include "Utils.h"
#include <GLFW/glfw3.h>

/* class for GLFW window managment
* handles most of the functions for glfw
*/

class GLFWApp
{
public:
	GLFWApp(GlobalState& state);
	~GLFWApp();
	GLFWwindow* getWindow() const { return window_; }
	float getDeltaSeconds() const { return deltaSeconds_; }
	void swapBuffers()
	{
		glfwSwapBuffers(window_);
		glfwPollEvents();
		assert(glGetError() == GL_NO_ERROR);

		const double newTimeStamp = glfwGetTime();
		deltaSeconds_ = static_cast<float>(newTimeStamp - timeStamp_);
		timeStamp_ = newTimeStamp;
	}

	// should toggle between fullscreen and window mode
	void updateWindow()
	{
		
		if (globalState->fullscreen_)
		{
			const GLFWvidmode* info = glfwGetVideoMode(glfwGetPrimaryMonitor());
			globalState->width = info->width;
			globalState->height = info->height;
			glfwSetWindowSize(window_, globalState->width, globalState->height);
			glfwSetWindowPos(window_, 0, 0);
		}else{
			int w, h;
			glfwGetFramebufferSize(window_, &w, &h);
			globalState->width = w;
			globalState->height = h;
			}
	}

private:
	GlobalState* globalState = nullptr;
	GLFWwindow* window_ = nullptr;
	double timeStamp_ = glfwGetTime();
	float deltaSeconds_ = 0.0f;
};

GLFWApp::GLFWApp(GlobalState& state)
{
	glfwSetErrorCallback(
		[](int error, const char* description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}
	);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	globalState = &state;

	// GLFW should use OpenGL Version 4.6 with core functions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_REFRESH_RATE, globalState->refresh_rate);
	glfwWindowHint(GLFW_SAMPLES, 4);

	
	

	window_ = glfwCreateWindow(globalState->width, globalState->height, globalState->window_title.c_str(), nullptr, nullptr);

	if (!window_)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window_);
	glfwSwapInterval(0);
}

GLFWApp::~GLFWApp()
{
	glfwDestroyWindow(window_);
	glfwTerminate();
}