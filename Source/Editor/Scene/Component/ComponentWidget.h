#pragma once

#include <string>
#include <imgui/imgui.h>

#include "Editor/EditorWidget.h"

namespace Arg
{
	class ComponentWidget : public EditorWidget
	{
	public:
		ComponentWidget(Vec2 position, Vec2 size)
			: EditorWidget(position, size)
		{

		}

		void Draw() override
		{
			if (ImGui::CollapsingHeader(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				OnDrawProperties();
			}
		}

		virtual void OnDrawProperties() = 0;

	protected:
		virtual std::string GetName() const = 0;
	};
}
