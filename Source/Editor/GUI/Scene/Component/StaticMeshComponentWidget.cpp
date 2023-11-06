#include "StaticMeshComponentWidget.h"

#include "Gameplay/Scene/Scene.h"
#include "Gameplay/Scene/GameObject/GameObject.h"
#include "Gameplay/Scene/Component/StaticMeshComponent.h"
#include "Resources/ResourceTypes/TextureResource.h"

Arg::StaticMeshComponentWidget::StaticMeshComponentWidget(StaticMeshComponent* component)
	: ComponentWidget(Vec2(0.0f), Vec2(1.0f)),
	m_pComponent(component)
{

}

void Arg::StaticMeshComponentWidget::VDrawProperties()
{
	Vec3 color = m_pComponent->GetColor();
	ImGui::ColorEdit3("Color", Math::Ref(color));
	m_pComponent->SetColor(color);

	const GUID textureID = m_pComponent->GetTextureID();
	Content* content = m_pComponent->GetOwner()->GetScene()->GetContent();

	std::string textureName;
	if (textureID == GUID::Empty)
	{
		textureName = "Empty";
	}
	else
	{
		TextureResource* texRes = content->GetResource<TextureResource>(textureID);
		textureName = texRes->GetName();
	}

	ImGui::Text("Texture: %s", textureName.c_str());

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TextureResourceID"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GUID));
			GUID* payload_data = (GUID*)(payload->Data);
			m_pComponent->SetTextureID(*payload_data);
		}
		ImGui::EndDragDropTarget();
	}
}

void Arg::StaticMeshComponentWidget::VOnRemoveClicked()
{
	Scene* scene = m_pComponent->GetOwner()->GetScene();
	scene->DestroyComponent(m_pComponent->GetID());
}
