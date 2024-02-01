#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"
#include "Renderer/RenderContext.hpp"
#include "Content/Serialization/YamlSerializable.hpp"
#include "Gameplay/GameTime.hpp"
#include "Content/ResourceCache.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class Actor;
		class ComponentRegistry;

		class ActorComponent : public Content::YamlSerializable
		{
		public:
			ActorComponent() = default;
			virtual ~ActorComponent() = default;

			virtual auto VCreateDefault() -> std::shared_ptr<ActorComponent> = 0;

			auto GetOwner() const -> Actor* { return m_pOwner; }
			void SetOwner(Actor* actor) { m_pOwner = actor; }

			virtual auto VGetID() const -> GUID = 0;
			virtual auto VGetName() const -> const std::string& = 0;

			virtual void VBeginPlay()
			{
			}

			virtual void VTick(const GameTime& gameTime)
			{
			}

			virtual void VRender(Renderer::RenderContext& context)
			{
			}

			virtual void VDrawDebug(Renderer::RenderContext& context)
			{
			}

			virtual void VOnComponentAdded() = 0;
			virtual void VOnComponentRemoved() = 0;

			virtual auto VIsScriptable() -> bool { return false; }

			auto CanEverTick() const -> const bool& { return m_bCanEverTick; }
			auto CanEverRender() const -> const bool& { return m_bCanEverRender; }

		protected:
			virtual auto VOnSerialize(YAML::Node& node) const -> bool override { return true; }
			virtual auto VOnDeserialize(const YAML::Node& node) -> bool override { return true; }

			auto GetResourceCache() const -> Content::ResourceCache*;
			auto GetComponentRegistry() const -> ComponentRegistry*;

		protected:
			bool m_bCanEverTick = true;
			bool m_bCanEverRender = true;

		private:
			Actor* m_pOwner = nullptr;
		};
	}
}
