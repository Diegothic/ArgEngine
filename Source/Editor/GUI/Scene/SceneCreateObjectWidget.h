#pragma once

#include "Editor/GUI/EditorWidget.h"

namespace Arg
{
	class Scene;

	class SceneCreateObjectWidget : public EditorWidget
	{
	public:
		SceneCreateObjectWidget(Vec2 position, Vec2 size, Scene* scene);

	protected:
		void VOnDraw() override;

	private:
		Scene* m_pScene = nullptr;
	};
}
