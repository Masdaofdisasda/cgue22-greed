#include "LoadingScreen.h"
#include "Texture.h"

LoadingScreen::LoadingScreen(GLFWApp* app, int width, int height)
{
	window = std::make_shared<GLFWApp>(*app);
	w = width;
	h = height;


	scrn0 = Texture::loadTexture("../../assets/textures/loading/screen0.jpg");
	scrn25 = Texture::loadTexture("../../assets/textures/loading/screen25.jpg");
	scrn50 = Texture::loadTexture("../../assets/textures/loading/screen50.jpg");
	scrn75 = Texture::loadTexture("../../assets/textures/loading/screen75.jpg");
	scrn100 = Texture::loadTexture("../../assets/textures/loading/screen100.jpg");

	Shader renderImgVert("../../assets/shaders/fullScreenTriangle.vert");
	Shader renderImgFrag("../../assets/shaders/HUD/fullScreenImage.frag");
	renderImage.buildFrom(renderImgVert, renderImgFrag);

	glCreateVertexArrays(1, &emptyVAO);

}

void LoadingScreen::init(GLFWApp* app, int width, int height)
{
}

void LoadingScreen::DrawProgress()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, w, h);

	renderImage.Use();
	glBindTextureUnit(9, *screens[step]);
	glBindVertexArray(emptyVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	std::cout << "loop = " << step << std::endl;

	window->swapBuffersUnsafe();
	step++;
	if (5 == step)
	{
		end = true;
	}
}
/*
void LoadingScreen::loop() //TODO
{
	GLenum err = glGetError();
	Shader renderImgVert("../../assets/shaders/fullScreenTriangle.vert");
	 err = glGetError();
	Shader renderImgFrag("../../assets/shaders/HUD/fullScreenImage.frag");
	 err = glGetError();

	
	renderImage->buildFrom(renderImgVert, renderImgFrag);

	err = glGetError();
	while (!end)
	{
		DrawProgress();
	}
}
*/
LoadingScreen::~LoadingScreen()
{
	glDeleteTextures(1, &scrn0);
	glDeleteTextures(1, &scrn25);
	glDeleteTextures(1, &scrn50);
	glDeleteTextures(1, &scrn75);
	glDeleteTextures(1, &scrn100);
}
