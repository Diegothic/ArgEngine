#include "EditorWidget.h"


Arg::EditorWidget::EditorWidget(Vec2 position, Vec2 size)
	: m_Position(position),
	m_Size(size)
{

}

void Arg::EditorWidget::Draw()
{
	VOnDraw();
}

void Arg::EditorWidget::Resize(Vec2 position, Vec2 size)
{
	m_Position = position;
	m_Size = size;

	VOnResized(position, size);
}
