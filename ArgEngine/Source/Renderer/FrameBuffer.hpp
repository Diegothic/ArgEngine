#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Renderer
	{
		class FrameBuffer
		{
		public:
			FrameBuffer();
			FrameBuffer(const FrameBuffer&) = delete;
			~FrameBuffer();

			auto GetRendererID() const -> const uint32_t& { return m_RendererID; }
			void DisableColorBuffers() const;
			void Bind() const;
			void Unbind() const;

		private:
			uint32_t m_RendererID = 0;
		};
	}
}
