#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Renderer
	{
		class VertexBuffer
		{
		public:
			VertexBuffer();
			VertexBuffer(const VertexBuffer&) = delete;
			~VertexBuffer();

			void SetData(const void* data, uint64_t size) const;
			void Bind() const;
			void Unbind() const;

		private:
			static uint32_t s_CurrentBoundID;

		private:
			uint32_t m_RendererID = 0;
		};

		class IndexBuffer
		{
		public:
			IndexBuffer();
			IndexBuffer(const IndexBuffer&) = delete;
			~IndexBuffer();

			void SetData(const void* data, uint64_t count) const;
			void Bind() const;
			void Unbind() const;

		private:
			static uint32_t s_CurrentBoundID;

		private:
			uint32_t m_RendererID = 0;
		};
	}
}
