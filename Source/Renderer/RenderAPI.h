#pragma once

#include "Arg/Math.h"

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
	};
}