#pragma once

#include <string>

#include "Editor/GUI/EditorWidget.h"

namespace Arg
{
	class ComponentWidget : public EditorWidget
	{
	public:
		ComponentWidget(Vec2 position, Vec2 size);

	protected:
		void VOnDraw() override;

	protected:
		virtual void VDrawProperties() = 0;
		virtual std::string VGetDisplayName() const = 0;
		virtual void VOnRemoveClicked() = 0;
	};
}
