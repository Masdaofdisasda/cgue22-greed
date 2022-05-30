#include "LoadingScreen.h"
#include "Texture.h"

LoadingScreen::LoadingScreen(glfw_app* app, const int width, const int height)
{
	window_ = std::make_shared<glfw_app>(*app);
	w_ = width;
	h_ = height;

	init();
}

void LoadingScreen::init()
{
	scrn0_ = Texture::load_texture("../../assets/textures/loading/screen0.ktx");
	scrn25_ = Texture::load_texture("../../assets/textures/loading/screen25.ktx");
	scrn50_ = Texture::load_texture("../../assets/textures/loading/screen50.ktx");
	scrn75_ = Texture::load_texture("../../assets/textures/loading/screen75.ktx");
	scrn100_ = Texture::load_texture("../../assets/textures/loading/screen100.ktx");

	render_image_ = std::make_unique<program>();
	Shader render_img_vert("../../assets/shaders/fullScreenTriangle.vert");
	Shader render_img_frag("../../assets/shaders/HUD/fullScreenImage.frag");
	render_image_->build_from(render_img_vert, render_img_frag);

	glCreateVertexArrays(1, &empty_vao_);
}

void LoadingScreen::draw_progress()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, w_, h_);

	render_image_->use();
	glBindTextureUnit(16, *screens_[step_]);
	glBindVertexArray(empty_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	Sleep(100);

	window_->swap_buffers();
	step_++;
	if (5 == step_)
	{
		end_ = true;
	}
}


void LoadingScreen::start() //TODO
{

	while (!end_)
	{
		draw_progress();
	}
}


LoadingScreen::~LoadingScreen()
{
	window_ = nullptr;
	glDeleteTextures(1, &scrn0_);
	glDeleteTextures(1, &scrn25_);
	glDeleteTextures(1, &scrn50_);
	glDeleteTextures(1, &scrn75_);
	glDeleteTextures(1, &scrn100_);
}
