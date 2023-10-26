#pragma once

#include "ComponentWidget.h"
#include "Gameplay/Scene/Components/StaticMeshComponent.h"

namespace Arg
{
	class StaticMeshComponentWidget : public ComponentWidget
	{
	public:
		StaticMeshComponentWidget(StaticMeshComponent* component)
			: ComponentWidget(Vec2(0.0f), Vec2(1.0f)),
			m_pComponent(component)
		{

		}

		virtual void OnDrawProperties() override
		{
			Vec3 color = m_pComponent->GetColor();
			ImGui::ColorEdit3("Color", Math::Ref(color));
			m_pComponent->SetColor(color);
		}

	protected:
		virtual std::string GetName() const override
		{
			return "Static Mesh Component";
		}

	private:
		StaticMeshComponent* m_pComponent = nullptr;
	};
}
