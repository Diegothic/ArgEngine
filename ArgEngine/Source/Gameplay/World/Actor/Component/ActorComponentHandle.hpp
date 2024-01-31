#pragma once

#include <arg_pch.hpp>

#include "Gameplay/World/GameWorld.hpp"

namespace Arg
{
	namespace Gameplay
	{
		template <typename TComponentType>
		class ActorComponentHandle
		{
		public:
			ActorComponentHandle() = default;
			ActorComponentHandle(
				GameWorld* pWorld,
				GUID ownerID,
				GUID componentID
			);
			ActorComponentHandle(const ActorComponentHandle& other);
			~ActorComponentHandle() = default;

			auto operator=(const ActorComponentHandle& other) -> ActorComponentHandle&;
			auto operator==(const ActorComponentHandle& other) const -> bool;

			auto GetOwnerID() const -> const GUID& { return m_OwnerID; }
			auto GetComponentID() const -> const GUID& { return m_ComponentID; }
			auto IsValid() const -> bool;
			auto Get() const -> TComponentType&;

		private:
			GameWorld* m_pWorld = nullptr;
			GUID m_OwnerID = GUID::Empty;
			GUID m_ComponentID = GUID::Empty;
		};

		template <typename TComponentType>
		ActorComponentHandle<TComponentType>::ActorComponentHandle(
			GameWorld* pWorld,
			GUID ownerID,
			GUID componentID
		)
			: m_pWorld(pWorld),
			  m_OwnerID(ownerID),
			  m_ComponentID(componentID)
		{
		}

		template <typename TComponentType>
		ActorComponentHandle<TComponentType>::ActorComponentHandle(
			const ActorComponentHandle& other
		)
		{
			*this = other;
		}

		template <typename TComponentType>
		auto ActorComponentHandle<TComponentType>::operator=(
			const ActorComponentHandle& other
		) -> ActorComponentHandle&
		{
			this->m_pWorld = other.m_pWorld;
			this->m_OwnerID = other.m_OwnerID;
			this->m_ComponentID = other.m_ComponentID;

			return *this;
		}

		template <typename TComponentType>
		auto ActorComponentHandle<TComponentType>::operator==(
			const ActorComponentHandle& other
		) const -> bool
		{
			return m_OwnerID == other.m_OwnerID && m_ComponentID == other.m_ComponentID;
		}

		template <typename TComponentType>
		auto ActorComponentHandle<TComponentType>::IsValid() const -> bool
		{
			return m_ComponentID != GUID::Empty
				&& m_OwnerID != GUID::Empty
				&& m_pWorld->HasActor(m_OwnerID)
				&& m_pWorld->GetActor(m_OwnerID).HasComponent(m_ComponentID);
		}

		template <typename TComponentType>
		auto ActorComponentHandle<TComponentType>::Get() const -> TComponentType&
		{
			ARG_ASSERT(IsValid());
			return *(std::dynamic_pointer_cast<TComponentType>(
				m_pWorld->GetActor(m_OwnerID).GetComponent(m_ComponentID)
			));
		}
	}
}
