#pragma once

#include "Editor/GUI/EditorWidget.h"

namespace Arg
{
	class Scene;
	class GameObject;
	class Inspector;

	class InspectorWidget : public EditorWidget
	{
	public:
		InspectorWidget(Vec2 position, Vec2 size, Inspector* inspector, Scene* scene);

	protected:
		void VOnDraw() override;
		void VOnResized(Vec2 newPosition, Vec2 newSize) override;

	private:
		void DrawGameObjectInspector(GameObject* gameObject) const;

	private:
		Inspector* m_pInspector = nullptr;
		Scene* m_pScene = nullptr;
	};
}
