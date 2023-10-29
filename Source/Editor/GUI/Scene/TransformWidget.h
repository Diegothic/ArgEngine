#pragma once

#include "Editor/GUI/EditorWidget.h"

namespace Arg
{
	class GameObject;

	class TransformWidget : public EditorWidget
	{
	public:
		TransformWidget(GameObject* gameObject);

	protected:
		void VOnDraw() override;

	private:
		GameObject* m_pGameObject = nullptr;
	};
}
