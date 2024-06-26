﻿#include <arg_pch.hpp>
#include "ActorHandle.hpp"

#include "Gameplay/World/GameWorld.hpp"

Arg::Gameplay::ActorHandle::ActorHandle(GameWorld* pWorld, GUID actorID)
	: m_pWorld(pWorld),
	  m_ActorID(actorID)
{
}

Arg::Gameplay::ActorHandle::ActorHandle(const ActorHandle& other)
{
	*this = other;
}

auto Arg::Gameplay::ActorHandle::operator=(const ActorHandle& other) -> ActorHandle&
{
	m_pWorld = other.m_pWorld;
	m_ActorID = other.m_ActorID;
	return *this;
}

auto Arg::Gameplay::ActorHandle::operator==(const ActorHandle& other) const -> bool
{
	return m_ActorID == other.m_ActorID;
}

auto Arg::Gameplay::ActorHandle::IsValid() const -> bool
{
	return m_ActorID != GUID::Empty && m_pWorld != nullptr && m_pWorld->HasActor(m_ActorID);
}

auto Arg::Gameplay::ActorHandle::Get() const -> Actor&
{
	ARG_ASSERT(IsValid());
	return m_pWorld->GetActor(m_ActorID);
}
