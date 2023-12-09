#include <arg_pch.hpp>
#include "ActorHandle.hpp"

#include "Gameplay/World/GameWorld.hpp"

Arg::Gameplay::ActorHandle::ActorHandle(const GUID ID, GameWorld* world)
	: m_ActorID(ID),
	m_pWorld(world)
{

}

auto Arg::Gameplay::ActorHandle::IsValid() const -> bool
{
	return true;
}

auto Arg::Gameplay::ActorHandle::Get() const->std::shared_ptr<Actor>
{
	return m_pWorld->GetActor(m_ActorID);
}

auto Arg::Gameplay::ActorHandle::operator==(const ActorHandle& other) const -> bool
{
	return m_ActorID == other.m_ActorID;
}

auto Arg::Gameplay::ActorHandle::operator=(const ActorHandle& other) ->ActorHandle&
{
	m_ActorID = other.m_ActorID;
	m_pWorld = other.m_pWorld;
	return *this;
}

Arg::Gameplay::ActorHandle::ActorHandle(const ActorHandle& other)
{
	*this = other;
}
