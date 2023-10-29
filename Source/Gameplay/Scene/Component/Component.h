#pragma once

#include <cstdint>

#include "Core/Time/GameTime.h"
#include "Renderer/RenderContext.h"

namespace Arg
{
	class GameObject;
	class ComponentWidget;

	class Component
	{
	public:
		Component(uint64_t ID, GameObject* owner);
		virtual ~Component() = default;
		bool operator==(const Component& other) const;

		uint64_t GetID() const { return m_ID; }
		GameObject* GetOwner() const { return m_pOwner; }

		virtual void Tick(const GameTime& gameTime) = 0;
		virtual void Render(const RenderContext& renderContext) = 0;
		void Destroy();

		bool IsMarkedForDestruction() const { return m_IsDestroyed; }

		virtual Rc<ComponentWidget> CreateWidget() = 0;

	private:
		uint64_t m_ID = 0;
		GameObject* m_pOwner = nullptr;

		bool m_IsDestroyed = false;
	};
}
