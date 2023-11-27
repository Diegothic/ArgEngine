#include <arg_pch.hpp>
#include "StaticMesh.hpp"

#include <glad/glad.h>

uint32_t Arg::Renderer::StaticMesh::s_CurrentBoundID = -1;

Arg::Renderer::StaticMesh::StaticMesh()
{
	glGenVertexArrays(1, &m_RendererID);
	Bind();
	m_VertexBuffer.Bind();
	m_VertexBuffer.SetData(nullptr, 0);
	m_IndexBuffer.Bind();
	m_IndexBuffer.SetData(nullptr, 0);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(StaticVertex),
		nullptr
	);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(StaticVertex),
		(void*)offsetof(StaticVertex, Normal)
	);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(StaticVertex),
		(void*)offsetof(StaticVertex, TexUV)
	);

	Unbind();
	m_VertexBuffer.Unbind();
	m_IndexBuffer.Unbind();
}

Arg::Renderer::StaticMesh::~StaticMesh()
{
	Unbind();
	glDeleteVertexArrays(1, &m_RendererID);
}

void Arg::Renderer::StaticMesh::SetData(const StaticMeshData& data)
{
	Bind();
	m_IndexCount = data.Indices.size();
	m_VertexBuffer.Bind();
	m_VertexBuffer.SetData(
		data.Vertices.data(),
		data.Vertices.size() * sizeof(StaticVertex)
	);
	m_IndexBuffer.Bind();
	m_IndexBuffer.SetData(
		data.Indices.data(),
		m_IndexCount
	);
}

void Arg::Renderer::StaticMesh::Bind() const
{
	if (s_CurrentBoundID == m_RendererID)
	{
		return;
	}

	s_CurrentBoundID = m_RendererID;
	glBindVertexArray(m_RendererID);
}

void Arg::Renderer::StaticMesh::Unbind() const
{
	s_CurrentBoundID = -1;
	glBindVertexArray(0);
	m_VertexBuffer.Unbind();
	m_IndexBuffer.Unbind();
}

void Arg::Renderer::StaticMesh::Draw() const
{
	Bind();
	glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexCount), GL_UNSIGNED_INT, nullptr);
	Unbind();
}
