#include <arg_pch.hpp>
#include "SkeletalMesh.hpp"

#include <glad/glad.h>

uint32_t Arg::Renderer::SkeletalMesh::s_CurrentBoundID = -1;

Arg::Renderer::SkeletalMesh::SkeletalMesh()
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
		sizeof(SkeletalVertex),
		nullptr
	);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SkeletalVertex),
		(void*)offsetof(SkeletalVertex, Normal)
	);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SkeletalVertex),
		(void*)offsetof(SkeletalVertex, TexUV)
	);

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(
		3,
		4,
		GL_INT,
		sizeof(SkeletalVertex),
		(void*)offsetof(SkeletalVertex, BoneIndices)
	);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SkeletalVertex),
		(void*)offsetof(SkeletalVertex, BoneWeights)
	);

	Unbind();
	m_VertexBuffer.Unbind();
	m_IndexBuffer.Unbind();
}

Arg::Renderer::SkeletalMesh::~SkeletalMesh()
{
	Unbind();
	glDeleteVertexArrays(1, &m_RendererID);
}

void Arg::Renderer::SkeletalMesh::SetData(const SkeletalMeshData& data)
{
	Bind();
	m_IndexCount = data.Indices.size();
	m_VertexBuffer.Bind();
	m_VertexBuffer.SetData(
		data.Vertices.data(),
		data.Vertices.size() * sizeof(SkeletalVertex)
	);
	m_IndexBuffer.Bind();
	m_IndexBuffer.SetData(
		data.Indices.data(),
		m_IndexCount
	);
}

void Arg::Renderer::SkeletalMesh::Bind() const
{
	if (s_CurrentBoundID == m_RendererID)
	{
		return;
	}

	s_CurrentBoundID = m_RendererID;
	glBindVertexArray(m_RendererID);
}

void Arg::Renderer::SkeletalMesh::Unbind() const
{
	s_CurrentBoundID = -1;
	glBindVertexArray(0);
	m_VertexBuffer.Unbind();
	m_IndexBuffer.Unbind();
}

void Arg::Renderer::SkeletalMesh::Draw() const
{
	Bind();
	glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexCount), GL_UNSIGNED_INT, nullptr);
	Unbind();
}
