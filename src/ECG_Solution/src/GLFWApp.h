#pragma once

#include "Utils.h"
#include <GLFW/glfw3.h>
#include <optick/optick.h>

/// @brief GLFW window managemnt, handles most of the functions for glfw
class glfw_app
{
public:
	explicit glfw_app(const std::shared_ptr<global_state>& state);
	~glfw_app();
	GLFWwindow* get_window() const { return window_; }
	float get_delta_seconds() const { return delta_seconds_; }

	/// @brief swaps back & front buffer and checks for errors (for performance reasons only in debug build)
	void swap_buffers()
	{
		OPTICK_PUSH("swap draw buffers")
		glfwSwapBuffers(window_);
		OPTICK_POP()
		glfwPollEvents();
#ifdef _DEBUG
		OPTICK_PUSH("check glGetError")
			assert(glGetError() == GL_NO_ERROR);
		OPTICK_POP()
#endif

		const double new_time_stamp = glfwGetTime();
		delta_seconds_ = static_cast<float>(new_time_stamp - time_stamp_);
		time_stamp_ = new_time_stamp;
	}

	/// @brief should toggle between fullscreen and window mode
	void update_window()
	{
		if (state_->fullscreen)
		{
			const GLFWvidmode* info = glfwGetVideoMode(glfwGetPrimaryMonitor());
			state_->width = info->width;
			state_->height = info->height;
			glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0, state_->width, state_->height, state_->refresh_rate);
		}
	}

private:
	std::shared_ptr<global_state> state_;
	GLFWwindow* window_ = nullptr;
	double time_stamp_ = glfwGetTime();
	float delta_seconds_ = 0.0f;
};

inline glfw_app::glfw_app(const std::shared_ptr<global_state>& state)
{
	glfwSetErrorCallback(
		[](int error, const char* description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}
	);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	this->state_ = state;

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

inline glfw_app::~glfw_app()
{
	glfwDestroyWindow(window_);
	glfwTerminate();
}