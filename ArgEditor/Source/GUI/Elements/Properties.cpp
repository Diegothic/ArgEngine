#include <arg_pch.hpp>
#include "Properties.hpp"

#include <imgui/ImGui.hpp>

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

void Arg::Editor::GUI::ActorHandleProperty(
	const char* ID,
	Vec2 size,
	const char* actorName,
	const std::function<void(GUID)>& onActorDropped,
	const std::function<void()>& onActorCleared
)
{
	ImGui::PushID(ID);

	const auto cursorPos = ImGui::GetCursorPos();
	ImGui::Button(
		ID,
		ImVec2(size.x, size.y)
	);

	bool bHasDroppedActor = false;
	GUID droppedActorID;
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Actor"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GUID));
			droppedActorID = *((GUID*)payload->Data);
			bHasDroppedActor = true;
		}

		ImGui::EndDragDropTarget();
	}

	if (bHasDroppedActor)
	{
		onActorDropped(droppedActorID);
	}

	ImGui::SetCursorPos(ImVec2(cursorPos.x + size.x, cursorPos.y));
	if (ImGui::Button(
		"X",
		ImVec2(size.y, size.y)
	))
	{
		onActorCleared();
	}

	ImGui::SetCursorPos(ImVec2(cursorPos.x + 10.0f, cursorPos.y + 5.0f));

	if (actorName != nullptr)
	{
		ImGui::Text(actorName);
	}
	else
	{
		ImGui::TextDisabled("Empty reference");
	}

	ImGui::PopID();
}


void Arg::Editor::GUI::ActorComponentHandleProperty(
	const char* ID,
	Vec2 size,
	const char* actorComponentName,
	const std::function<void(GUID)>& onComponentDropped,
	const std::function<void()>& onComponentCleared
)
{
	ImGui::PushID(ID);

	const auto cursorPos = ImGui::GetCursorPos();
	ImGui::Button(
		ID,
		ImVec2(size.x, size.y)
	);

	bool bHasDroppedActor = false;
	GUID droppedActorID;
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload;
		if ((payload = ImGui::AcceptDragDropPayload("ActorComponent")) != nullptr)
		{
			IM_ASSERT(payload->DataSize == 2 * sizeof(GUID));
			droppedActorID = ((GUID*)payload->Data)[0];
			bHasDroppedActor = true;
		}
		else if ((payload = ImGui::AcceptDragDropPayload("Actor")) != nullptr)
		{
			IM_ASSERT(payload->DataSize == sizeof(GUID));
			droppedActorID = *((GUID*)payload->Data);
			bHasDroppedActor = true;
		}

		ImGui::EndDragDropTarget();
	}

	if (bHasDroppedActor)
	{
		onComponentDropped(droppedActorID);
	}

	ImGui::SetCursorPos(ImVec2(cursorPos.x + size.x, cursorPos.y));
	if (ImGui::Button(
		"X",
		ImVec2(size.y, size.y)
	))
	{
		onComponentCleared();
	}

	ImGui::SetCursorPos(ImVec2(cursorPos.x + 10.0f, cursorPos.y + 5.0f));

	if (actorComponentName != nullptr)
	{
		ImGui::Text(actorComponentName);
	}
	else
	{
		ImGui::TextDisabled("Empty reference");
	}

	ImGui::PopID();
}
