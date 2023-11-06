#pragma once

#include "Component.h"
#include "Core/GUID.h"

namespace Arg
{
	class StaticMeshComponent : public Component
	{
	public:
		StaticMeshComponent(uint64_t ID, GameObject* owner);

		void Tick(const GameTime& gameTime) override {}
		void Render(const RenderContext& renderContext) override;

		Rc<ComponentWidget> CreateWidget() override;

		Vec3 GetColor() const { return m_Color; }
		void SetColor(Vec3 color) { m_Color = color; }

		GUID GetTextureID() const { return m_TextureID; }
		void SetTextureID(GUID ID) { m_TextureID = ID; }

	private:
		uint32_t m_VertexArrayHandle;
		Vec3 m_Color = Vec3(1.0f);
		GUID m_TextureID = GUID::Empty;
	};
}
