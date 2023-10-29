#include "ComponentWidget.h"

Arg::ComponentWidget::ComponentWidget(Vec2 position, Vec2 size) : EditorWidget(position, size)
{

}

void Arg::ComponentWidget::VOnDraw()
{
	const bool isHeaderOpened = ImGui::CollapsingHeader(
		VGetDisplayName().c_str(),
		ImGuiTreeNodeFlags_DefaultOpen
	);
	if (ImGui::BeginPopupContextItem("Component Actions"))
	{
		if (ImGui::MenuItem("Remove"))
		{
			VOnRemoveClicked();
		}
		ImGui::EndPopup();
	}

	if (isHeaderOpened)
	{
		VDrawProperties();
	}
}
