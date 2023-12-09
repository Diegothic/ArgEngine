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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	const bool isOpen = ImGui::Begin("World Outliner", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		if (pGameEngine->IsWorldLoaded())
		{
			auto& world = pGameEngine->GetLoadedWorld();
			const auto& rootActor = world->GetRootActor();
			const bool isOpen = ImGui::CollapsingHeader("##WorldHeader",
				ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_FramePadding
				| ImGuiTreeNodeFlags_AllowOverlap
			);

			ImGui::SameLine(28.0f);
			ImGui::SetCursorPosY(28.0f);
			const uint32_t imageID = worldTexture->GetRendererID();
			ImGui::Image((void*)(intptr_t)imageID, ImVec2(16.0f, 16.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			ImGui::SameLine(50.0f);
			ImGui::Text(world->GetName().c_str());

			ImGui::SameLine(ImGui::GetWindowSize().x - 100.0f);
			if (ImGui::Button("New Actor", ImVec2(80.0f, 24.0f)))
			{
				//world->CreateActor(rootActor);
			}

			if (isOpen)
			{
				DrawActorTree(context, rootActor, 0);
			}
		}
	}

	ImGui::End();
}

void Arg::Editor::GUI::WorldOutlinerPanel::DrawActorTree(
	const EditorGUIContext& context,
	const std::shared_ptr<Gameplay::Actor>& actor,
	const int32_t treeLevel
)
{
	auto& pEditor = context.pEditor;

	const auto& actorTexture = m_ActorTexture.Get()->GetTexture();

	for (size_t i = 0; i < actor->GetChildActorsCount(); i++)
	{
		ImGui::PushID(static_cast<int32_t>(i));
		const auto& childActor = actor->GetChildActor(i);
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
		if (pEditor->HasSelectedActor())
		{
			auto pSelectedActor = pEditor->GetSelectedActor();
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

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			pEditor->SelectActor(childActor->GetID());
		}

		ImGui::SameLine(treeLevel * 20.0f + 30.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
		const uint32_t imageID = actorTexture->GetRendererID();
		ImGui::Image((void*)(intptr_t)imageID, ImVec2(16.0f, 16.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		ImGui::SameLine(treeLevel * 20.0f + 30.0f + 25.0f);
		ImGui::Text(childActor->GetName().c_str());

		if (isOpen)
		{
			DrawActorTree(context, childActor, treeLevel + 1);

			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}
