#pragma once

#include <GL\glew.h>
#include "Texture.h"
#include <memory>

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

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, handle_);
	glViewport(0, 0, width_, height_);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}