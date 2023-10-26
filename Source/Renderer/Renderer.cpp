#include "Renderer.h"

Arg::Renderer::Renderer(const RendererSpec& spec)
	: m_pWindowHandle(spec.WindowHandle)
{
}

bool Arg::Renderer::Initialize()
{
	m_GraphicsContext = NewBox<GraphicsContext>((GLFWwindow*)m_pWindowHandle);
	m_RenderAPI = NewBox<RenderAPI>();

	const bool contextInitialized = m_GraphicsContext->Initialize();
	if (!contextInitialized)
	{
		return false;
	}

	return true;
}

void Arg::Renderer::BeginFrame(const FrameParams& params)
{
	m_FrameParams = params;

	m_RenderAPI->SetViewport(Vec2i(0), params.Size);
	m_RenderAPI->SetClearColor(Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	m_RenderAPI->SetDepthTestEnabled(true);
	m_RenderAPI->ClearBuffer();
}

void Arg::Renderer::EndFrame()
{
	m_GraphicsContext->SwapBuffers();
}
