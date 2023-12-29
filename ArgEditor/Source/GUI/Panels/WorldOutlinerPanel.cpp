#include <arg_pch.hpp>
#include "WorldOutlinerPanel.hpp"

#include <imgui/imgui.h>

#include "Editor.hpp"

void Arg::Editor::GUI::WorldOutlinerPanel::OnInitialize(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;
	auto& pResourceCache = pEditor->GetResourceCache();

	m_WorldTexture = pResourceCache->CreateHandle<Content::TextureResource>(
		"Icons\\world_icon"
	);
	m_WorldTexture.AddRef();

	m_ActorTexture = pResourceCache->CreateHandle<Content::TextureResource>(
		"Icons\\actor_icon"
	);
	m_ActorTexture.AddRef();
}

void Arg::Editor::GUI::WorldOutlinerPanel::OnDraw(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;
	auto& pContent = pEditor->GetContent();
	auto& pResourceCache = pEditor->GetResourceCache();
	auto& pGameEngine = pEditor->GetGameEngine();

	const auto& worldTexture = m_WorldTexture.Get()->GetTexture();
	m_bHasClickedActor = false;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	const bool isOpen = ImGui::Begin("World Outliner", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		if (!pGameEngine->IsWorldLoaded())
		{
			ImGui::Text("No world opened!");
			ImGui::End();
			return;
		}

		auto& pWorld = pGameEngine->GetLoadedWorld();
		Gameplay::Actor* pRootActor = &pWorld->GetRootActor();
		const bool isHeaderOpen = ImGui::CollapsingHeader(
			"##WorldHeader",
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowOverlap
		);

		ImGui::SameLine(28.0f);
		ImGui::SetCursorPosY(28.0f);
		const uint32_t imageID = worldTexture->GetRendererID();
		ImGui::Image(
			(void*)(intptr_t)imageID,
			ImVec2(16.0f, 16.0f),
			ImVec2(0.0f, 1.0f),
			ImVec2(1.0f, 0.0f)
		);
		ImGui::SameLine(50.0f);
		ImGui::Text(pWorld->GetName().c_str());

		ImGui::SameLine(ImGui::GetWindowSize().x - 100.0f);
		if (ImGui::Button("New Actor", ImVec2(80.0f, 24.0f)))
		{
			const GUID newActorID = pWorld->CreateActor();
			Gameplay::Actor& newActor = pWorld->GetActor(newActorID);
			newActor.SetName("New Actor");
		}

		if (isHeaderOpen)
		{
			DrawActorTree(context, pRootActor, 0);
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
			{
				if (payload->IsDataType("Actor"))
				{
					const GUID droppedActorID = *((GUID*)payload->Data);
					Gameplay::Actor& actor = pWorld->GetActor(droppedActorID);
					pWorld->ReparentActor(actor, pWorld->GetRootActor());
				}
			}
			else if (!m_bHasClickedActor)
			{
				pEditor->DeselectActor();
			}
		}
	}

	ImGui::End();
}

void Arg::Editor::GUI::WorldOutlinerPanel::DrawActorTree(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	const int32_t treeLevel
)
{
	auto& pEditor = context.pEditor;
	auto& pGameEngine = pEditor->GetGameEngine();
	if (!pGameEngine->IsWorldLoaded())
	{
		return;
	}
	auto& pWorld = pGameEngine->GetLoadedWorld();

	const auto& actorTexture = m_ActorTexture.Get()->GetTexture();

	for (size_t i = 0; i < pActor->GetChildActorsCount(); i++)
	{
		ImGui::PushID(static_cast<int32_t>(i));
		const auto& childActor = pActor->GetChildActor(i);
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_Framed;
		const bool isLeaf = childActor->GetChildActorsCount() < 1;
		if (isLeaf)
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}

		bool bIsSelected = false;
		Gameplay::Actor* pSelectedActor = nullptr;
		if (pEditor->HasSelectedActor()
			&& pEditor->GetSelectedActor(pSelectedActor))
		{
			if (pSelectedActor == childActor)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
				bIsSelected = true;
			}
		}

		if (bIsSelected)
		{
			ImGui::PushStyleColor(
				ImGuiCol_Header,
				ImVec4(0.27f, 0.84f, 0.93f, 0.3f)
			);
			ImGui::PushStyleColor(
				ImGuiCol_HeaderHovered,
				ImVec4(0.37f, 0.94f, 1.0f, 0.3f)
			);
			ImGui::PushStyleColor(
				ImGuiCol_HeaderActive,
				ImVec4(0.17f, 0.74f, 0.83f, 0.3f)
			);
		}

		const bool isOpen = ImGui::TreeNodeEx("##ActorNode", nodeFlags);
		if (bIsSelected)
		{
			ImGui::PopStyleColor(3);
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Actor", &childActor->GetID(), sizeof(GUID));
			ImGui::Text(childActor->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		bool droppedActor = false;
		GUID droppedActorID = GUID::Empty;
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Actor"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GUID));
				droppedActorID = *((GUID*)payload->Data);
				droppedActor = true;
			}
			ImGui::EndDragDropTarget();
		}

		if (droppedActor && droppedActorID != GUID::Empty)
		{
			Gameplay::Actor& actor = pWorld->GetActor(droppedActorID);
			pWorld->ReparentActor(actor, *childActor);
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			pEditor->SelectActor(childActor->GetID());
			m_bHasClickedActor = true;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(ImGui::GetID("##ActorContextMenu"));
		}

		static bool isRename = false;
		static GUID renamedActor;
		if (ImGui::BeginPopupContextItem("##FolderContextMenu"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				isRename = true;
				renamedActor = childActor->GetID();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("New Actor"))
			{
				const GUID newActorID = pWorld->CreateActor(*childActor);
				Gameplay::Actor& newActor = pWorld->GetActor(newActorID);
				newActor.SetName("New Child Actor");
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Remove"))
			{
				if (bIsSelected)
				{
					pEditor->DeselectActor();
				}

				childActor->MarkForDestruction();
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine(treeLevel * 20.0f + 30.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
		const uint32_t imageID = actorTexture->GetRendererID();
		ImGui::Image((void*)(intptr_t)imageID, ImVec2(16.0f, 16.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		ImGui::SameLine(treeLevel * 20.0f + 30.0f + 25.0f);
		static bool doRename = false;
		static std::string newActorName;
		if (isRename && renamedActor == childActor->GetID())
		{
			ImGui::SetKeyboardFocusHere();
			static char buffer[1024];
			strcpy_s(buffer, childActor->GetName().c_str());
			ImGui::InputText(
				"##ActorNewName",
				buffer,
				1024,
				ImGuiInputTextFlags_CallbackAlways,
				[](ImGuiInputTextCallbackData* data) -> int32_t
				{
					if (data->BufTextLen < 1)
					{
						return 0;
					}

					if (ImGui::IsKeyDown(ImGuiKey_Escape))
					{
						isRename = false;
						return 1;
					}

					if (ImGui::IsKeyDown(ImGuiKey_Enter))
					{
						newActorName = data->Buf;
						doRename = true;
						isRename = false;
						return 1;
					}

					return 0;
				}
			);
		}
		else
		{
			ImGui::Text(childActor->GetName().c_str());
		}

		if (doRename)
		{
			doRename = false;
			childActor->SetName(newActorName);
		}

		if (isOpen)
		{
			DrawActorTree(context, childActor, treeLevel + 1);

			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}
