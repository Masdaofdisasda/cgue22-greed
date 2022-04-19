#pragma once

#include "GLFWApp.h"

#ifndef _LOADING_SCREEN_
#define _LOADING_SCREEN_
class LoadingScreen
{
public:
	LoadingScreen(GLFWApp* app, int width, int height);
	void DrawProgress();
	~LoadingScreen();

private:
	GLFWApp* window;
	int w;
	int h;

	int step;

	GLuint scrn0 = 0;
	GLuint scrn25 = 0;
	GLuint scrn50 = 0;
	GLuint scrn75 = 0;
	GLuint scrn100 = 0;

	GLuint* screens[5] = { &scrn0,&scrn25, &scrn50,&scrn75, &scrn100 };

	Program renderImage;
};

LoadingScreen::LoadingScreen(GLFWApp* app, int width, int height)
{
	window = app;
	w = width;
	h = height;
	step = 0;
	scrn0 = Texture::loadTexture("../../assets/textures/loading/screen0.jpg");
	scrn25 = Texture::loadTexture("../../assets/textures/loading/screen25.jpg");
	scrn50 = Texture::loadTexture("../../assets/textures/loading/screen50.jpg");
	scrn75 = Texture::loadTexture("../../assets/textures/loading/screen75.jpg");
	scrn100 = Texture::loadTexture("../../assets/textures/loading/screen100.jpg");



	Shader renderImgVert("../../assets/shaders/fullScreenTriangle.vert");
	Shader renderImgFrag("../../assets/shaders/HUD/fullScreenImage.frag");
	renderImage.buildFrom(renderImgVert, renderImgFrag);

}

inline void LoadingScreen::DrawProgress()
{
	if (step < 5)
	{
		GLuint emptyVAO;
		glCreateVertexArrays(1, &emptyVAO);
		glViewport(0, 0, w, h);
		renderImage.Use();
		glBindTextureUnit(9, *screens[step]);
		glBindVertexArray(emptyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		window->swapBuffers();
		step++;
	}
}



LoadingScreen::~LoadingScreen()
{
	glDeleteTextures(1, &scrn0);
	glDeleteTextures(1, &scrn25);
	glDeleteTextures(1, &scrn50);
	glDeleteTextures(1, &scrn75);
	glDeleteTextures(1, &scrn100);
}

#endif