#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"
#include "Transform.hpp"
#include "Component/ComponentRegistry.hpp"
#include "Content/Serialization/YamlSerializable.hpp"
#include "Gameplay/GameTime.hpp"
#include "Gameplay/GameContext.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class GameWorld;

		class Actor : public Content::YamlSerializable
		{
		public:
			Actor(GUID ID, GameWorld* world);
			virtual ~Actor() = default;

			auto operator==(const Actor& other) const { return m_ID == other.m_ID; }

			auto GetID() const -> const GUID& { return m_ID; }
			void SetID(const GUID ID) { m_ID = ID; }
			auto GetName() const -> const std::string& { return m_Name; }
			void SetName(const std::string& name);
			auto GetWorld() const -> GameWorld* { return m_pWorld; }

			template <typename TComponentType>
			auto GetComponent() -> std::shared_ptr<TComponentType>&;
			template <typename TComponentType>
			auto AddComponent() -> bool;
			template <typename TComponentType>
			auto RemoveComponent() -> bool;

			auto GetComponentCount() const -> size_t;
			auto HasComponent(const GUID& componentID) const -> bool;
			auto GetComponent(const GUID& componentID) -> std::shared_ptr<ActorComponent>&;
			auto GetComponentByIndex(const size_t index) -> std::shared_ptr<ActorComponent>&;
			auto AddComponent(const std::shared_ptr<ActorComponent>& component) -> bool;
			auto RemoveComponent(const GUID& componentID) -> bool;

			auto GetParentActor() const -> Actor* { return m_pParentActor; }
			void SetParentActor(Actor* actor);
			auto GetChildActorsCount() const -> size_t { return m_pChildActors.size(); }
			auto GetChildActor(const size_t index) const -> Actor* { return m_pChildActors[index]; }
			void AddChildActor(Actor* actor);
			void RemoveChildActor(const Actor* actor);
			void ClearChildActors();

			auto GetTransform() const -> const Mat4& { return m_GlobalTransform; }
			void UpdateTransform(const Mat4& parentTransform);
			void ReparentTransform(const Actor& newParentActor);

			auto GetLocalPosition() const -> const Vec3& { return m_Transform.GetPosition(); }
			void SetLocalPosition(const Vec3& position);
			auto GetLocalRotation() const -> const Quat& { return m_Transform.GetRotation(); }
			void SetLocalRotation(const Quat& rotation);
			auto GetLocalRotationEuler() const -> const Vec3& { return m_Transform.GetRotationEuler(); }
			void SetLocalRotationEuler(const Vec3& rotationEuler);
			auto GetLocalScale() const -> const Vec3& { return m_Transform.GetScale(); }
			void SetLocalScale(const Vec3& scale);

			auto GetPosition() const -> Vec3;
			void SetPosition(const Vec3& position);
			auto GetRotation() const -> Quat;
			void SetRotation(const Quat& rotation);
			auto GetRotationEuler() const -> Vec3;
			void SetRotationEuler(const Vec3& rotationEuler);
			auto GetScale() const -> Vec3;
			void SetScale(const Vec3& scale);

			auto GetForwardVec() const -> Vec3;
			auto GetRightVec() const -> Vec3;
			auto GetUpVec() const -> Vec3;

			void BeginPlay();
			void Tick(const GameTime& gameTime, const GameInput& gameInput);
			void Render(Renderer::RenderContext& context);
			void DrawDebug(Renderer::RenderContext& context);

			void MarkForDestruction() { m_bIsDestroyed = true; }
			auto IsMarkedForDestruction() const -> bool { return m_bIsDestroyed; }
			void Destroy();

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

			void RefreshTransform(const Mat4& parentTransform);

			auto GetResourceCache() const -> Content::ResourceCache*;
			auto GetComponentRegistry() const -> ComponentRegistry*;

		private:
			GUID m_ID = GUID::Empty;
			std::string m_Name = "Actor";

			Transform m_Transform;
			Mat4 m_GlobalTransform = Math::MAT4_IDENTITY;
			bool m_bRefreshTransform = false;
			bool m_bIsDestroyed = false;

			GameWorld* m_pWorld = nullptr;

			std::vector<std::shared_ptr<ActorComponent>> m_Components;
			std::unordered_map<GUID, std::shared_ptr<ActorComponent>> m_ComponentsRegistry;

			Actor* m_pParentActor = nullptr;
			std::vector<Actor*> m_pChildActors;
		};

		template <typename TComponentType>
		auto Actor::GetComponent() -> std::shared_ptr<TComponentType>&
		{
			const GUID componentID = TComponentType::COMPONENT_ID;
			return *(std::dynamic_pointer_cast<TComponentType>(GetComponent(componentID)));
		}

		template <typename TComponentType>
		auto Actor::AddComponent() -> bool
		{
			const std::shared_ptr<ActorComponent> component = TComponentType::CreateDefault();
			return AddComponent(component);
		}

		template <typename TComponentType>
		auto Actor::RemoveComponent() -> bool
		{
			const GUID componentID = TComponentType::COMPONENT_ID;
			return RemoveComponent(componentID);
		}
	}
}
