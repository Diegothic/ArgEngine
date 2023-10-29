#pragma once

#include "Editor/GUI/EditorWidget.h"

namespace Arg
{
	class Scene;
	class GameObject;
	class Inspector;

	class SceneHierarchyWidget : public EditorWidget
	{
	public:
		SceneHierarchyWidget(Vec2 position, Vec2 size, Scene* scene, Inspector* inspector);

	protected:
		void VOnDraw() override;

	private:
		void SelectGameObject(uint64_t ID) const;
		void DrawTransformWithChildren(GameObject* gameObject);
		void OnItemDropped(uint64_t ID, uint64_t targetID = 0) const;

	private:
		Scene* m_pScene = nullptr;
		Inspector* m_pInspector = nullptr;
	};
}
