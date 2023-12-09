#include <arg_pch.hpp>
#include "ActorComponent.hpp"

#include "Gameplay/World/GameWorld.hpp"

auto Arg::Gameplay::ActorComponent::GetResourceCache() const->Content::ResourceCache*
{
	return GetOwner()->GetWorld()->GetResourceCache();
}

auto Arg::Gameplay::ActorComponent::GetComponentRegistry() const->ComponentRegistry*
{
	return GetOwner()->GetWorld()->GetComponentRegistry();
}
