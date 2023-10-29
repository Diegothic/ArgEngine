#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Core/Math/MathTypes.h"

namespace Arg
{
	class EditorWidget
	{
	public:
		EditorWidget(Vec2 position, Vec2 size);
		virtual ~EditorWidget() = default;

		const Vec2& GetPosition() const { return m_Position; }
		const Vec2& GetSize() const { return m_Size; }

		void Draw();
		void Resize(Vec2 position, Vec2 size);

	protected:
		virtual void VOnDraw() = 0;
		virtual void VOnResized(Vec2 newPosition, Vec2 newSize) {}

	private:
		Vec2 m_Position;
		Vec2 m_Size;
	};
}
