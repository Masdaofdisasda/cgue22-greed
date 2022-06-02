#include "LoadingScreen.h"
#include "Texture.h"

LoadingScreen::LoadingScreen(const int width, const int height)
	: w_(width), h_(height)
{
	init();
}

void LoadingScreen::init()
{
	logo_ = Texture::load_texture("../assets/shaders/HUD/loading_logo.ktx");

	scrn0_ = Texture::load_texture("../assets/shaders/HUD/screen0.ktx");
	scrn25_ = Texture::load_texture("../assets/shaders/HUD/screen25.ktx");
	scrn50_ = Texture::load_texture("../assets/shaders/HUD/screen50.ktx");
	scrn75_ = Texture::load_texture("../assets/shaders/HUD/screen75.ktx");
	scrn100_ = Texture::load_texture("../assets/shaders/HUD/screen100.ktx");

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
		glViewport((w_ - 784) * 0.5f, (h_ - 194)*0.5f, 784, 194);

		render_image_->use();
		glBindTextureUnit(16, logo_);
		glBindVertexArray(empty_vao_);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	if (5 == step_)
		return;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport((w_- 1457)*0.5f, h_*0.2, 1457, 94);

	render_image_->use();
	glBindTextureUnit(16, *screens_[step_]);
	glBindVertexArray(empty_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	Sleep(1000);
	
	step_++;
}



LoadingScreen::~LoadingScreen()
{
	glDeleteBuffers(1, &empty_vao_);
	glDeleteTextures(1, &logo_);
	glDeleteTextures(1, &scrn0_);
	glDeleteTextures(1, &scrn25_);
	glDeleteTextures(1, &scrn50_);
	glDeleteTextures(1, &scrn75_);
	glDeleteTextures(1, &scrn100_);
}
