#include <arg_pch.hpp>
#include "TriggerVolume.hpp"

#include "Debug/Assert.hpp"

Arg::Physics::TriggerVolume::TriggerVolume(const GUID& actorID)
	: m_ActorID(actorID)
{
}

void Arg::Physics::TriggerVolume::Initialize(int32_t userIndex)
{
	m_UserIndex = userIndex;
}

auto Arg::Physics::TriggerVolume::GetFlag(const TriggerVolumeFlag& flag) const -> bool
{
	return m_Flags & static_cast<int32_t>(flag);
}

void Arg::Physics::TriggerVolume::SetFlag(const TriggerVolumeFlag& flag, bool value)
{
	if (value)
	{
		m_Flags |= static_cast<int32_t>(flag);
	}
	else
	{
		m_Flags &= ~static_cast<int32_t>(flag);
	}
}

void Arg::Physics::TriggerVolume::BeginOverlap()
{
	m_TriggerEntered.clear();
	m_TriggerExit.clear();
	m_LastOverlaps = m_Overlaps;
	m_Overlaps.clear();
}

void Arg::Physics::TriggerVolume::AddOverlap(const int32_t& userIndex)
{
	m_Overlaps.push_back(userIndex);
}

void Arg::Physics::TriggerVolume::AddTriggerEntered(int32_t index)
{
	m_TriggerEntered.push_back(index);
}

void Arg::Physics::TriggerVolume::AddTriggerExit(int32_t index)
{
	m_TriggerExit.push_back(index);
}

auto Arg::Physics::TriggerVolume::HasOverlappedWith(const int32_t& userIndex) const -> bool
{
	return std::ranges::find(m_Overlaps, userIndex) != m_Overlaps.end();
}

auto Arg::Physics::TriggerVolume::HasOverlappedLastWith(const int32_t& userIndex) const -> bool
{
	return std::ranges::find(m_LastOverlaps, userIndex) != m_LastOverlaps.end();
}

auto Arg::Physics::TriggerVolume::GetOverlappedLastCount() const -> size_t
{
	return m_LastOverlaps.size();
}

auto Arg::Physics::TriggerVolume::GetOverlappedLast(size_t index) const -> const int32_t&
{
	ARG_ASSERT(index < m_LastOverlaps.size());
	return m_LastOverlaps[index];
}

auto Arg::Physics::TriggerVolume::GetTriggerEnteredCount() const -> size_t
{
	return m_TriggerEntered.size();
}

auto Arg::Physics::TriggerVolume::GetTriggerEntered(size_t index) const -> const int32_t&
{
	ARG_ASSERT(index < m_TriggerEntered.size());
	return m_TriggerEntered[index];
}

auto Arg::Physics::TriggerVolume::GetTriggerExitCount() const -> size_t
{
	return m_TriggerExit.size();
}

auto Arg::Physics::TriggerVolume::GetTriggerExit(size_t index) const -> const int32_t&
{
	ARG_ASSERT(index < m_TriggerExit.size());
	return m_TriggerExit[index];
}

void Arg::Physics::TriggerVolume::SetShape(const TriggerVolumeShape& shape)
{
	m_Shape = shape;
}

void Arg::Physics::TriggerVolume::SetSize(const Vec3& size)
{
	m_Size = size;
}
