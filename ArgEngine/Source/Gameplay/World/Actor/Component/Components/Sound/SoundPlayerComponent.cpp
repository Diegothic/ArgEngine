#include <arg_pch.hpp>
#include "SoundPlayerComponent.hpp"

#include "Gameplay/World/GameWorld.hpp"
#include "Gameplay/World/Actor/Actor.hpp"

const Arg::GUID Arg::Gameplay::SoundPlayerComponent::COMPONENT_ID
	= std::hash<std::string>{}("SoundPlayerComponent");

const std::string Arg::Gameplay::SoundPlayerComponent::COMPONENT_NAME = "SoundPlayerComponent";

Arg::Gameplay::SoundPlayerComponent::SoundPlayerComponent()
{
	m_bCanEverRender = false;
}

auto Arg::Gameplay::SoundPlayerComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<SoundPlayerComponent>();
}

void Arg::Gameplay::SoundPlayerComponent::VBeginPlay()
{
	ActorComponent::VBeginPlay();

	if (m_bPlayOnStart)
	{
		Play();
	}
}

void Arg::Gameplay::SoundPlayerComponent::VTick(const GameTime& gameTime)
{
	ActorComponent::VTick(gameTime);

	if (m_bIs3D && m_SoundHandle.IsValid())
	{
		const Vec3 OwnerPosition = GetOwner()->GetPosition();
		m_SoundHandle.UpdatePosition(OwnerPosition);
	}
}

void Arg::Gameplay::SoundPlayerComponent::VOnComponentAdded()
{
}

void Arg::Gameplay::SoundPlayerComponent::VOnComponentRemoved()
{
}

void Arg::Gameplay::SoundPlayerComponent::Play()
{
	if (m_SoundHandle.IsValid())
	{
		Stop();
	}

	if (!m_SoundResource.IsValid())
	{
		return;
	}

	Sound::SoundEngine& sound = GetOwner()->GetWorld()->GetSound();
	if (m_bIs3D)
	{
		m_Spec.Position = GetOwner()->GetPosition();
		m_SoundHandle = sound.PlaySound3D(
			m_SoundResource,
			m_Spec
		);
	}
	else
	{
		m_SoundHandle = sound.PlaySound2D(
			m_SoundResource,
			m_Spec
		);
	}
}

void Arg::Gameplay::SoundPlayerComponent::Pause()
{
	ARG_ASSERT(m_SoundHandle.IsValid());
	m_SoundHandle.Pause();
}

void Arg::Gameplay::SoundPlayerComponent::Unpause()
{
	ARG_ASSERT(m_SoundHandle.IsValid());
	m_SoundHandle.Play();
}

void Arg::Gameplay::SoundPlayerComponent::Stop()
{
	ARG_ASSERT(m_SoundHandle.IsValid());
	m_SoundHandle.Stop();
}

auto Arg::Gameplay::SoundPlayerComponent::IsPlaying() const -> bool
{
	ARG_ASSERT(m_SoundHandle.IsValid());
	return m_SoundHandle.IsPlaying();
}

auto Arg::Gameplay::SoundPlayerComponent::GetSound() const -> SoundResourceHandle
{
	return m_SoundResource;
}

void Arg::Gameplay::SoundPlayerComponent::SetSound(SoundResourceHandle soundHandle)
{
	if (m_SoundHandle.IsValid())
	{
		Stop();
	}

	m_SoundResource = soundHandle;
}

auto Arg::Gameplay::SoundPlayerComponent::GetPlayOnStart() const -> bool
{
	return m_bPlayOnStart;
}

void Arg::Gameplay::SoundPlayerComponent::SetPlayOnStart(bool bPlayOnStart)
{
	m_bPlayOnStart = bPlayOnStart;
}

auto Arg::Gameplay::SoundPlayerComponent::GetVolume() const -> float
{
	return m_Spec.Volume;
}

void Arg::Gameplay::SoundPlayerComponent::SetVolume(float volume)
{
	m_Spec.Volume = volume;
	if (m_SoundHandle.IsValid())
	{
		m_SoundHandle.UpdateVolume(m_Spec.Volume);
	}
}

auto Arg::Gameplay::SoundPlayerComponent::GetVolumeVariance() const -> float
{
	return m_Spec.VolumeVariance;
}

void Arg::Gameplay::SoundPlayerComponent::SetVolumeVariance(float variance)
{
	m_Spec.VolumeVariance = variance;
}

auto Arg::Gameplay::SoundPlayerComponent::GetPitch() const -> float
{
	return m_Spec.Pitch;
}

void Arg::Gameplay::SoundPlayerComponent::SetPitch(float pitch)
{
	m_Spec.Pitch = pitch;
	if (m_SoundHandle.IsValid())
	{
		m_SoundHandle.UpdatePitch(m_Spec.Pitch);
	}
}

auto Arg::Gameplay::SoundPlayerComponent::GetPitchVariance() const -> float
{
	return m_Spec.PitchVariance;
}

void Arg::Gameplay::SoundPlayerComponent::SetPitchVariance(float variance)
{
	m_Spec.PitchVariance = variance;
}

auto Arg::Gameplay::SoundPlayerComponent::GetIsLooping() const -> bool
{
	return m_Spec.bIsLooping;
}

void Arg::Gameplay::SoundPlayerComponent::SetIsLooping(bool bIsLooping)
{
	m_Spec.bIsLooping = bIsLooping;
	if (m_SoundHandle.IsValid())
	{
		m_SoundHandle.UpdateLooping(m_Spec.bIsLooping);
	}
}

auto Arg::Gameplay::SoundPlayerComponent::GetIs3D() const -> bool
{
	return m_bIs3D;
}

void Arg::Gameplay::SoundPlayerComponent::SetIs3D(bool bIs3D)
{
	m_bIs3D = bIs3D;
}

auto Arg::Gameplay::SoundPlayerComponent::GetDistance() const -> float
{
	return m_Spec.Distance;
}

void Arg::Gameplay::SoundPlayerComponent::SetDistance(float distance)
{
	distance = Math::max(distance, 0.0f);
	m_Spec.Distance = distance;
	if (m_SoundHandle.IsValid())
	{
		m_SoundHandle.UpdateDistance(m_Spec.Distance);
	}
}

bool Arg::Gameplay::SoundPlayerComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["SoundID"] = m_SoundResource.GetID();

	node["PlayOnStart"] = m_bPlayOnStart;
	node["Is3D"] = m_bIs3D;
	node["Volume"] = m_Spec.Volume;
	node["VolumeVariance"] = m_Spec.VolumeVariance;
	node["Pitch"] = m_Spec.Pitch;
	node["PitchVariance"] = m_Spec.PitchVariance;
	node["Looping"] = m_Spec.bIsLooping;
	node["Distance"] = m_Spec.Distance;

	return true;
}

bool Arg::Gameplay::SoundPlayerComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	const GUID soundID = ValueOr<GUID>(node["SoundID"], GUID::Empty);
	m_SoundResource = GetResourceCache()->CreateHandle<Content::SoundResource>(soundID);

	m_bPlayOnStart = ValueOr<bool>(node["PlayOnStart"], true);
	m_bIs3D = ValueOr<bool>(node["Is3D"], true);
	m_Spec.Volume = ValueOr<float>(node["Volume"], 1.0f);
	m_Spec.VolumeVariance = ValueOr<float>(node["VolumeVariance"], 0.0f);
	m_Spec.Pitch = ValueOr<float>(node["Pitch"], 1.0f);
	m_Spec.PitchVariance = ValueOr<float>(node["PitchVariance"], 0.0f);
	m_Spec.bIsLooping = ValueOr<bool>(node["Looping"], false);
	m_Spec.Distance = ValueOr<float>(node["Distance"], 50.0f);

	return true;
}
