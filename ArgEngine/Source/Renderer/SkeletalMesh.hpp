#pragma once

#include <arg_pch.hpp>

#include "Buffers.hpp"
#include "Vertex.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct SkeletalMeshData
		{
			std::vector<SkeletalVertex> Vertices;
			std::vector<uint32_t> Indices;
		};

		class SkeletalMesh
		{
		public:
			SkeletalMesh();
			SkeletalMesh(const SkeletalMesh&) = delete;
			~SkeletalMesh();

			void SetData(const SkeletalMeshData& data);
			void Bind() const;
			void Unbind() const;

			void Draw() const;

		private:
			static uint32_t s_CurrentBoundID;

		private:
			uint32_t m_RendererID = -1;
			uint64_t m_IndexCount = 0;

			VertexBuffer m_VertexBuffer;
			IndexBuffer m_IndexBuffer;
		};
	}
}