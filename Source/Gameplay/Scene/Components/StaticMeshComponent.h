#pragma once

#include "Component.h"

namespace Arg
{
	class StaticMeshComponent : public Component
	{
	public:
		StaticMeshComponent(uint64_t ID, GameObject* owner);

		void Tick(const GameTime& gameTime) override {}
		void Render(const RenderContext& renderContext) override;

	private:
		uint32_t m_VertexArrayHandle;
	};
}
