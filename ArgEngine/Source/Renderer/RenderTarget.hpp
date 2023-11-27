#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "FrameBuffer.hpp"
#include "Texture.hpp"

namespace Arg
{
	namespace Renderer
	{
		class RenderTarget
		{
		public:
			RenderTarget();
			RenderTarget(const RenderTarget&) = delete;
			~RenderTarget() = default;

			void Begin() const;
			void End() const;

			auto GetRendererID() const -> const uint32_t&;
			auto GetSize() const -> const Vec2i& { return m_Size; }
			void SetSize(const Vec2i& size);

		private:
			Vec2i m_Size = Vec2i(1,1);
			FrameBuffer m_Buffer;
			Texture m_ColorAttachment;
			Texture m_StencilDepthAttachment;
		};
	}
}
