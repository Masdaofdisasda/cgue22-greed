#pragma once
#include "Utils.h"
#include <GLFW/glfw3.h>

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

	// todo fullscreen
	void updateWindow(std::string title)
	{
		glfwSetWindowTitle(window_, title.c_str());
		if (globalState->_fullscreen)
		{
			const GLFWvidmode* info = glfwGetVideoMode(glfwGetPrimaryMonitor());
			globalState->width = info->width;
			globalState->height = info->height;
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