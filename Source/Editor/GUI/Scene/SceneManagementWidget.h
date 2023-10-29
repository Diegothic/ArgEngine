#pragma once

#include "Core/Memory/SmartPtr.h"
#include "Editor/GUI/EditorWidget.h"

namespace Arg
{
	class Scene;
	class Inspector;

	class SceneManagementWidget : public EditorWidget
	{
	public:
		SceneManagementWidget(Vec2 position, Vec2 size, Scene* scene, Inspector* inspector);

	protected:
		void VOnDraw() override;
		void VOnResized(Vec2 newPosition, Vec2 newSize) override;

	private:
		Scene* m_pScene = nullptr;
		Inspector* m_pInspector = nullptr;
		Box<EditorWidget> m_HierarchyWidget = nullptr;
		Box<EditorWidget> m_CreateObjectWidget = nullptr;
	};
}
