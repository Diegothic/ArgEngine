#include <arg_pch.hpp>
#include "Properties.hpp"

#include <imgui/imgui.h>

void Arg::Editor::GUI::ResourceHandleProperty(
	const char* ID,
	Vec2 size,
	const char* resourceName,
	const std::function<void(GUID)>& onResourceDropped,
	const std::function<void()>& onResourceCleared
)
{
	ImGui::PushID(ID);

	const auto cursorPos = ImGui::GetCursorPos();
	ImGui::Button(
		ID,
		ImVec2(size.x, size.y)
	);

	bool bHasDroppedResource = false;
	GUID droppedResourceID;
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GUID));
			droppedResourceID = *((GUID*)payload->Data);
			bHasDroppedResource = true;
		}

		ImGui::EndDragDropTarget();
	}

	if (bHasDroppedResource)
	{
		onResourceDropped(droppedResourceID);
	}

	ImGui::SetCursorPos(ImVec2(cursorPos.x + size.x, cursorPos.y));
	if (ImGui::Button(
		"X",
		ImVec2(size.y, size.y)
	))
	{
		onResourceCleared();
	}

	ImGui::SetCursorPos(ImVec2(cursorPos.x + 10.0f, cursorPos.y + 5.0f));

	if (resourceName != nullptr)
	{
		ImGui::Text(resourceName);
	}
	else
	{
		ImGui::TextDisabled("Empty reference");
	}

	ImGui::PopID();
}
