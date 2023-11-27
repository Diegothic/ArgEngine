#include <arg_pch.hpp>
#include "FrameBuffer.hpp"

#include <glad/glad.h>

Arg::Renderer::FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &m_RendererID);
}

Arg::Renderer::FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_RendererID);
}

void Arg::Renderer::FrameBuffer::DisableColorBuffers() const
{
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void Arg::Renderer::FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

void Arg::Renderer::FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
