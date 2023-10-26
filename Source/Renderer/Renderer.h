#pragma once

#include "Memory/SmartPtr.h"
#include "GraphicsContext.h"
#include "RenderAPI.h"

namespace Arg
{
	struct RendererSpec
	{
		void* WindowHandle;
	};

	struct FrameParams
	{
		Vec4 ClearColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		Vec2i Size = Vec2i(1920, 1080);
	};

	class Renderer
	{
	public:
		Renderer(const RendererSpec& spec);
		~Renderer() = default;

		bool Initialize();

		void BeginFrame(const FrameParams& params) const;
		void EndFrame() const;

	private:
		void* m_pWindowHandle;
		Box<GraphicsContext> m_GraphicsContext;
		Box<RenderAPI> m_RenderAPI;
	};
}
