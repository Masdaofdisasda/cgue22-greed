#pragma once

#include "Texture.h"

/// @brief Framebuffer is for offscreen rendering and contains a color or depth buffer
class Framebuffer
{
public:
	Framebuffer(int width, int height, GLenum formatColor, GLenum formatDepth);
	~Framebuffer();
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&&) = default;
	GLuint getHandle() const { return handle_; }
	const Texture& getTextureColor() const { return *texColor_.get(); }
	const Texture& getTextureDepth() const { return *texDepth_.get(); }
	void bind();
	void unbind();

private:
	int width_;
	int height_;
	GLuint handle_ = 0;

	std::unique_ptr<Texture> texColor_;
	std::unique_ptr<Texture> texDepth_;
};

/// @brief generate a new framebuffer and assigns needed textures
/// @param width of the framebuffer
/// @param height of the framebuffer
/// @param formatColor is the color type eg. GL_RGBA16F
/// @param formatDepth is the depth type eg. GL_DEPTH_COMPONENT24
Framebuffer::Framebuffer(int width, int height, GLenum formatColor, GLenum formatDepth)
	: width_(width)
	, height_(height)
{
	glCreateFramebuffers(1, &handle_);

	if (formatColor)
	{
		texColor_ = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, formatColor);
		glTextureParameteri(texColor_->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texColor_->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0, texColor_->getHandle(), 0);
	}
	if (formatDepth)
	{
		texDepth_ = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, formatDepth);
		const GLfloat border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTextureParameterfv(texDepth_->getHandle(), GL_TEXTURE_BORDER_COLOR, border);
		glTextureParameteri(texDepth_->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(texDepth_->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glNamedFramebufferTexture(handle_, GL_DEPTH_ATTACHMENT, texDepth_->getHandle(), 0);
	}

	const GLenum status = glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER);

	assert(status == GL_FRAMEBUFFER_COMPLETE);
}

Framebuffer::~Framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &handle_);
}

/// @brief binds the framebuffer, subsequent draw calls will be render to this framebuffer
/// caution: the viewport is automaticly set to the framebuffer size, the viewport state
/// prior to this call will be lost
void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, handle_);
	glViewport(0, 0, width_, height_); // sets viewport according to size
}

/// @brief unbinds framebuffer, subsequent draw calls can render on screen
/// caution: the viewport needs to be set manually after unbinding!
void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}