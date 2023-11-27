#include <arg_pch.hpp>
#include "Buffers.hpp"

#include <glad/glad.h>

uint32_t Arg::Renderer::VertexBuffer::s_CurrentBoundID = -1;
uint32_t Arg::Renderer::IndexBuffer::s_CurrentBoundID = -1;

Arg::Renderer::VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &m_RendererID);
}

Arg::Renderer::VertexBuffer::~VertexBuffer()
{
	Unbind();
	glDeleteBuffers(1, &m_RendererID);
}

void Arg::Renderer::VertexBuffer::SetData(const void* data, uint64_t size) const
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER,
		static_cast<int64_t>(size),
		data,
		GL_STATIC_DRAW
	);
}

void Arg::Renderer::VertexBuffer::Bind() const
{
	if (s_CurrentBoundID == m_RendererID)
	{
		return;
	}

	s_CurrentBoundID = m_RendererID;
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void Arg::Renderer::VertexBuffer::Unbind() const
{
	s_CurrentBoundID = -1;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Arg::Renderer::IndexBuffer::IndexBuffer()
{
	glGenBuffers(1, &m_RendererID);
}

Arg::Renderer::IndexBuffer::~IndexBuffer()
{
	Unbind();
	glDeleteBuffers(1, &m_RendererID);
}

void Arg::Renderer::IndexBuffer::SetData(const void* data, uint64_t count) const
{
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		static_cast<int64_t>(count * sizeof(uint32_t)),
		data,
		GL_STATIC_DRAW
	);
}

void Arg::Renderer::IndexBuffer::Bind() const
{
	if (s_CurrentBoundID == m_RendererID)
	{
		return;
	}

	s_CurrentBoundID = m_RendererID;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void Arg::Renderer::IndexBuffer::Unbind() const
{
	s_CurrentBoundID = -1;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
