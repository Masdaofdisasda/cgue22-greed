#pragma once

#include "GLFWApp.h"
#include "Program.h"

#ifndef _LOADING_SCREEN_
#define _LOADING_SCREEN_
/// @brief simulates what a loading screen might look like
/// hourse tried to make an actual loading screen: 11h
class LoadingScreen
{
public:
	 void start();
	 void stop() { end = true; }
	LoadingScreen(GLFWApp* app, int width, int height);
	~LoadingScreen();
	 void DrawProgress();

private:
	
	 void init();

	 std::shared_ptr<GLFWApp> window;
	    int w;
	    int h;
		bool end = false;

	    int step = 0;

		GLuint emptyVAO = 0;

	    GLuint scrn0 = 0;
	    GLuint scrn25 = 0;
	    GLuint scrn50 = 0;
	    GLuint scrn75 = 0;
	    GLuint scrn100 = 0;

		GLuint* screens[5] = { &scrn0,&scrn25, &scrn50,&scrn75, &scrn100 };

		std::shared_ptr<Program> renderImage;
};

#endif