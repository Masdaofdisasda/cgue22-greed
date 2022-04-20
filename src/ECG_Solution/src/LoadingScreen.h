#pragma once

#include "GLFWApp.h"
#include "Program.h"

#ifndef _LOADING_SCREEN_
#define _LOADING_SCREEN_
class LoadingScreen
{
public:
	 void init(GLFWApp* app, int width, int height);
	 void DrawProgress();
	 //void loop();
	 void stop() { end = true; }
	LoadingScreen(GLFWApp* app, int width, int height);
	~LoadingScreen();

private:
	
	 std::shared_ptr<GLFWApp> window;
	    int w;
	    int h;
		bool end = false;

	    int step;

		GLuint emptyVAO;

	    GLuint scrn0;
	    GLuint scrn25;
	    GLuint scrn50;
	    GLuint scrn75;
	    GLuint scrn100;

		GLuint* screens[5] = { &scrn0,&scrn25, &scrn50,&scrn75, &scrn100 };

		Program renderImage;
};

#endif