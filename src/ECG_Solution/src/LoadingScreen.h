#pragma once

#include "GLFWApp.h"
#include "Program.h"

/// @brief simulates what a loading screen might look like
/// hourse tried to make an actual loading screen: 11h
class LoadingScreen
{
public:
	 void start();
	 void stop() { end_ = true; }
	LoadingScreen(glfw_app* app, int width, int height);
	~LoadingScreen();
	 void draw_progress();

private:
	
	void init();

	glfw_app* window_;
	    int w_;
	    int h_;
		bool end_ = false;

	    int step_ = 0;

		GLuint empty_vao_ = 0;

		GLuint logo_ = 0;

	    GLuint scrn0_ = 0;
	    GLuint scrn25_ = 0;
	    GLuint scrn50_ = 0;
	    GLuint scrn75_ = 0;
	    GLuint scrn100_ = 0;

		GLuint* screens_[5] = { &scrn0_,&scrn25_, &scrn50_,&scrn75_, &scrn100_ };

		std::unique_ptr<program> render_image_;
};