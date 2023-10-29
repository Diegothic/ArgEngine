#pragma once

#include "ComponentWidget.h"

namespace Arg
{
	class StaticMeshComponent;

	class StaticMeshComponentWidget : public ComponentWidget
	{
	public:
		StaticMeshComponentWidget(StaticMeshComponent* component);

	protected:
		void VDrawProperties() override;
		std::string VGetDisplayName() const override
		{
			return "Static Mesh Component";
		}
		void VOnRemoveClicked() override;

	private:
		StaticMeshComponent* m_pComponent = nullptr;
	};
}
