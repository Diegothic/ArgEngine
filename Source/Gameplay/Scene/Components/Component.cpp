#include "Component.h"

#include "Gameplay/Scene/GameObject.h"

Arg::Component::Component(uint64_t ID, GameObject* owner) 
	: m_ID(ID),
	m_pOwner(owner)
{
	m_pOwner->AddComponent(this);
}

Arg::Component::~Component()
{
	m_pOwner->RemoveComponent(this);
}

bool Arg::Component::operator==(const Component& other) const
{
	return m_ID == other.m_ID;
}
