#pragma once
#include "Utils.h"
#include <GLFW/glfw3.h>

/// @brief GLFW window managemnt, handles most of the functions for glfw
class GLFWApp
{
public:
	GLFWApp(std::shared_ptr<GlobalState> state);
	~GLFWApp();
	GLFWwindow* getWindow() const { return window_; }
	float getDeltaSeconds() const { return deltaSeconds_; }

	/// @brief swaps back & front buffer and checks for errors
	void swapBuffers()
	{
		glfwSwapBuffers(window_);
		glfwPollEvents();
		assert(glGetError() == GL_NO_ERROR);

		const double newTimeStamp = glfwGetTime();
		deltaSeconds_ = static_cast<float>(newTimeStamp - timeStamp_);
		timeStamp_ = newTimeStamp;
	}

	/// @brief should toggle between fullscreen and window mode
	void updateWindow()
	{
		
		if (state->fullscreen_)
		{
			const GLFWvidmode* info = glfwGetVideoMode(glfwGetPrimaryMonitor());
			state->width = info->width;
			state->height = info->height;
			glfwSetWindowSize(window_, state->width, state->height);
			glfwSetWindowPos(window_, 0, 0);
		}else{
			int w, h;
			glfwGetFramebufferSize(window_, &w, &h);
			state->width = w;
			state->height = h;
			}
	}

private:
	std::shared_ptr<GlobalState> state;
	GLFWwindow* window_ = nullptr;
	double timeStamp_ = glfwGetTime();
	float deltaSeconds_ = 0.0f;
};

inline GLFWApp::GLFWApp(std::shared_ptr<GlobalState> state)
{
	glfwSetErrorCallback(
		[](int error, const char* description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}
	);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	this->state = state;

	// GLFW should use OpenGL Version 4.6 with core functions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_REFRESH_RATE, state->refresh_rate);
	glfwWindowHint(GLFW_SAMPLES, 4);

	
	

	window_ = glfwCreateWindow(state->width, state->height, state->window_title.c_str(), nullptr, nullptr);

	if (!window_)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window_);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glfwSwapInterval(0);
	glfwSwapBuffers(window_);
}

GLFWApp::~GLFWApp()
{
	glfwDestroyWindow(window_);
	glfwTerminate();
}