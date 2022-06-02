#include "LoadingScreen.h"
#include "Texture.h"

LoadingScreen::LoadingScreen(glfw_app* app, const int width, const int height)
{
	window_ = app;
	w_ = width;
	h_ = height;

	init();
}

void LoadingScreen::init()
{
	logo_ = Texture::load_texture("../assets/textures/loading/loading_logo.ktx");

	scrn0_ = Texture::load_texture("../assets/textures/loading/screen0.ktx");
	scrn25_ = Texture::load_texture("../assets/textures/loading/screen25.ktx");
	scrn50_ = Texture::load_texture("../assets/textures/loading/screen50.ktx");
	scrn75_ = Texture::load_texture("../assets/textures/loading/screen75.ktx");
	scrn100_ = Texture::load_texture("../assets/textures/loading/screen100.ktx");

	render_image_ = std::make_unique<program>();
	Shader render_img_vert("../assets/shaders/fullScreenTriangle.vert");
	Shader render_img_frag("../assets/shaders/HUD/fullScreenImage.frag");
	render_image_->build_from(render_img_vert, render_img_frag);

	glCreateVertexArrays(1, &empty_vao_);
}

void LoadingScreen::draw_progress()
{
	if (step_ == 0)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glViewport(w_ * 0.05, h_ * 0.08 + 40, 392, 97);

		render_image_->use();
		glBindTextureUnit(16, logo_);
		glBindVertexArray(empty_vao_);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(w_*0.05, h_*0.08, 392, 24);

	render_image_->use();
	glBindTextureUnit(16, *screens_[step_]);
	glBindVertexArray(empty_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 3);

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
	glDeleteBuffers(1, &empty_vao_);
	glDeleteTextures(1, &logo_);
	glDeleteTextures(1, &scrn0_);
	glDeleteTextures(1, &scrn25_);
	glDeleteTextures(1, &scrn50_);
	glDeleteTextures(1, &scrn75_);
	glDeleteTextures(1, &scrn100_);
}
