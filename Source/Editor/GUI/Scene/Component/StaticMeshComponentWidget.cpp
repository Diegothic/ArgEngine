#include "StaticMeshComponentWidget.h"

#include "Gameplay/Scene/Scene.h"
#include "Gameplay/Scene/GameObject/GameObject.h"
#include "Gameplay/Scene/Component/StaticMeshComponent.h"

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
}

void Arg::StaticMeshComponentWidget::VOnRemoveClicked()
{
	Scene* scene = m_pComponent->GetOwner()->GetScene();
	scene->DestroyComponent(m_pComponent->GetID());
}
