#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Renderer
	{
		namespace RenderAPI
		{
			void Initialize();
			void SetViewport(const Vec2i& position, const Vec2i& size);
			void SetClearColor(const Vec3& color);
			void ClearBuffers();

			void SetBlendingEnabled(bool bIsEnabled);
			void SetDrawingBackground(bool bDrawingBackground);
			void SetWireframe(bool bWireframe, float lineWidth = 1.0f);
			void SetCullBackFace(bool bCull);
		}
	}
}
