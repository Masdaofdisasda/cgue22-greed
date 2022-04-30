#pragma once

#include "Texture.h"

/// @brief Framebuffer is for offscreen rendering and contains a color or depth buffer
class framebuffer
{
public:
	framebuffer(int width, int height, GLenum format_color, GLenum format_depth);
	~framebuffer();
	framebuffer(const framebuffer&) = delete;
	framebuffer(framebuffer&&) = default;
	GLuint get_handle() const { return handle_; }
	const Texture& get_texture_color() const { return *tex_color_; }
	const Texture& get_texture_depth() const { return *tex_depth_; }
	void bind() const;
	static void unbind();

private:
	int width_;
	int height_;
	GLuint handle_ = 0;

	std::unique_ptr<Texture> tex_color_;
	std::unique_ptr<Texture> tex_depth_;
};

/// @brief generate a new framebuffer and assigns needed textures
/// @param width of the framebuffer
/// @param height of the framebuffer
/// @param format_color is the color type eg. GL_RGBA16F
/// @param format_depth is the depth type eg. GL_DEPTH_COMPONENT24
inline framebuffer::framebuffer(int width, int height, GLenum format_color, GLenum format_depth)
	: width_(width)
	, height_(height)
{
	glCreateFramebuffers(1, &handle_);

	if (format_color)
	{
		tex_color_ = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, format_color);
		glTextureParameteri(tex_color_->get_handle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(tex_color_->get_handle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0, tex_color_->get_handle(), 0);
	}
	if (format_depth)
	{
		tex_depth_ = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, format_depth);
		constexpr GLfloat border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTextureParameterfv(tex_depth_->get_handle(), GL_TEXTURE_BORDER_COLOR, border);
		glTextureParameteri(tex_depth_->get_handle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(tex_depth_->get_handle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glNamedFramebufferTexture(handle_, GL_DEPTH_ATTACHMENT, tex_depth_->get_handle(), 0);
	}

	const GLenum status = glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER);

	assert(status == GL_FRAMEBUFFER_COMPLETE);
}

inline framebuffer::~framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &handle_);
}

/// @brief binds the framebuffer, subsequent draw calls will be render to this framebuffer
/// caution: the viewport is automaticly set to the framebuffer size, the viewport state
/// prior to this call will be lost
inline void framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, handle_);
	glViewport(0, 0, width_, height_); // sets viewport according to size
}

/// @brief unbinds framebuffer, subsequent draw calls can render on screen
/// caution: the viewport needs to be set manually after unbinding!
inline void framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}