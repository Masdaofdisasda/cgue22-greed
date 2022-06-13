#pragma once

#include "Program.h"

/// @brief simulates what a loading screen might look like
/// hourse tried to make an actual loading screen: 11h
class LoadingScreen
{
public:
	LoadingScreen(int width, int height);
	~LoadingScreen();

	/**
	 * \brief draws the current loading progress
	 */
	void draw_progress();

private:

	/**
	 * \brief  loads loading screen images and compiles the needed shaders
	 */
	void init();
	
	    int w_;
	    int h_;

	    int step_ = 0;

		GLuint empty_vao_ = 0;

		GLuint logo_ = 0;

	    GLuint scrn0_ = 0;
	    GLuint scrn25_ = 0;
	    GLuint scrn50_ = 0;
	    GLuint scrn75_ = 0;
	    GLuint scrn100_ = 0;

		GLuint* screens_[5] = { &scrn0_,&scrn25_, &scrn50_,&scrn75_, &scrn100_ };

		std::shared_ptr<program> render_image_;
};