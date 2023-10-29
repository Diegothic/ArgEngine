#include "Component.h"

Arg::Component::Component(uint64_t ID, GameObject* owner)
	: m_ID(ID),
	m_pOwner(owner)
{
}

bool Arg::Component::operator==(const Component& other) const
{
	return m_ID == other.m_ID;
}

void Arg::Component::Destroy()
{
	if (m_IsDestroyed)
	{
		return;
	}

	m_IsDestroyed = true;
}
