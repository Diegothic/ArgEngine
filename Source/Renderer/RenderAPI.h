#pragma once

#include "Math/MathTypes.h"

namespace Arg
{
	class RenderAPI
	{
	public:
		RenderAPI() = default;
		~RenderAPI() = default;

		void ClearBuffer() const;
		void SetClearColor(Vec4 color) const;
		void SetViewport(Vec2i position, Vec2i size) const;
		void SetDepthTestEnabled(bool value) const;
	};
}
