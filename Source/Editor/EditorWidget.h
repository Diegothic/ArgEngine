#pragma once

#include "Math/MathTypes.h"

namespace Arg
{
	class EditorWidget
	{
	public:
		EditorWidget(Vec2 position, Vec2 size)
			: m_Position(position),
			m_Size(size)
		{}
		virtual ~EditorWidget() = default;

		const Vec2& GetPosition() const { return m_Position; }
		const Vec2& GetSize() const { return m_Size; }

		virtual void Draw() = 0;

		void Resize(Vec2 position, Vec2 size)
		{
			m_Position = position;
			m_Size = size;

			OnResized(position, size);
		}

		virtual void OnResized(Vec2 newPosition, Vec2 newSize) {}

	private:
		Vec2 m_Position;
		Vec2 m_Size;
	};
}
