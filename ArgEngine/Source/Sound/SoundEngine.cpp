#include "arg_pch.hpp"
#include "SoundEngine.hpp"

#include "Debug/Assert.hpp"

Arg::Sound::SoundHandle::SoundHandle(SoundEngine* pEngine, const GUID& ID)
	: m_pEngine(pEngine),
	  m_ID(ID)
{
}

auto Arg::Sound::SoundHandle::IsValid() const -> bool
{
	return m_ID != GUID::Empty
		&& m_pEngine != nullptr
		&& m_pEngine->GetSound(m_ID) != nullptr;
}

void Arg::Sound::SoundHandle::Play() const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->setIsPaused(false);
}

void Arg::Sound::SoundHandle::Pause() const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->setIsPaused(true);
}

void Arg::Sound::SoundHandle::Stop() const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->stop();
}

auto Arg::Sound::SoundHandle::IsPlaying() const -> bool
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	return !pSound->isFinished() && !pSound->getIsPaused();
}

void Arg::Sound::SoundHandle::UpdateVolume(float volume) const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->setVolume(volume);
}

void Arg::Sound::SoundHandle::UpdatePitch(float pitch) const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->setPlaybackSpeed(pitch);
}

void Arg::Sound::SoundHandle::UpdatePosition(const Vec3& position) const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->setPosition(irrklang::vec3df(position.x, -position.y, position.z));
}

void Arg::Sound::SoundHandle::UpdateDistance(float distance) const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->setMinDistance(distance);
}

void Arg::Sound::SoundHandle::UpdateLooping(bool value) const
{
	irrklang::ISound* pSound = m_pEngine->GetSound(m_ID);
	ARG_ASSERT(pSound != nullptr);
	pSound->setIsLooped(value);
}

void Arg::Sound::SoundEngine::Initialize()
{
	m_pEngine = std::unique_ptr<irrklang::ISoundEngine>(irrklang::createIrrKlangDevice());
	ARG_ASSERT(m_pEngine != nullptr);
}

void Arg::Sound::SoundEngine::CleanUp()
{
	for (const auto& pSound : m_Sounds | std::views::values)
	{
		pSound->drop();
	}

	m_Sounds.clear();
	m_pEngine->removeAllSoundSources();
	m_pEngine->drop();
	m_pEngine = nullptr;
}

void Arg::Sound::SoundEngine::ClearGarbage()
{
	std::vector<GUID> soundsToRemove;
	for (auto& sound : m_Sounds)
	{
		const GUID& ID = sound.first;
		irrklang::ISound* pSound = sound.second;
		if (pSound->isFinished())
		{
			soundsToRemove.push_back(ID);
		}
	}

	for (const GUID& ID : soundsToRemove)
	{
		m_Sounds.at(ID)->drop();
		m_Sounds.erase(ID);
	}
}

auto Arg::Sound::SoundEngine::PlaySound2D(
	const SoundResourceHandle& sound,
	const SoundSpec& spec
) -> SoundHandle
{
	sound.Get()->AddRef();

	std::string soundName = sound.Get()->GetName();
	soundName.append(sound.Get()->GetFormat());
	irrklang::ISoundSource* pSoundSource = m_pEngine->getSoundSource(
		soundName.c_str(),
		false
	);
	if (pSoundSource == nullptr)
	{
		char* pSoundData = sound.Get()->GetSoundData();
		const size_t pSoundDataSize = sound.Get()->GetSoundDataSize();

		pSoundSource = m_pEngine->addSoundSourceFromMemory(
			pSoundData,
			static_cast<int32_t>(pSoundDataSize),
			soundName.c_str()
		);
	}

	pSoundSource->setDefaultVolume(1.0f);
	irrklang::ISound* pSound = m_pEngine->play2D(
		pSoundSource,
		spec.bIsLooping,
		false,
		true
	);
	ARG_ASSERT(pSound != nullptr);
	pSound->setVolume(spec.Volume + m_Random.NextFloat() * spec.VolumeVariance);
	pSound->setPlaybackSpeed(spec.Pitch + m_Random.NextFloat() * spec.PitchVariance);

	sound.Get()->FreeRef();

	const GUID soundID = m_Random.Next();
	m_Sounds[soundID] = pSound;

	return {this, soundID};
}

auto Arg::Sound::SoundEngine::PlaySound3D(
	const SoundResourceHandle& sound,
	const SoundSpec& spec
) -> SoundHandle
{
	sound.Get()->AddRef();

	std::string soundName = sound.Get()->GetName();
	soundName.append(sound.Get()->GetFormat());
	irrklang::ISoundSource* pSoundSource = m_pEngine->getSoundSource(
		soundName.c_str(),
		false
	);
	if (pSoundSource == nullptr)
	{
		char* pSoundData = sound.Get()->GetSoundData();
		const size_t pSoundDataSize = sound.Get()->GetSoundDataSize();

		pSoundSource = m_pEngine->addSoundSourceFromMemory(
			pSoundData,
			static_cast<int32_t>(pSoundDataSize),
			soundName.c_str()
		);
	}

	pSoundSource->setDefaultVolume(1.0f);
	irrklang::ISound* pSound = m_pEngine->play3D(
		pSoundSource,
		irrklang::vec3df(spec.Position.x, -spec.Position.y, spec.Position.z),
		spec.bIsLooping,
		false,
		true
	);
	ARG_ASSERT(pSound != nullptr);
	pSound->setVolume(spec.Volume + m_Random.NextFloat() * spec.VolumeVariance);
	pSound->setPlaybackSpeed(spec.Pitch + m_Random.NextFloat() * spec.PitchVariance);
	pSound->setMinDistance(spec.Distance);

	sound.Get()->FreeRef();

	const GUID soundID = m_Random.Next();
	m_Sounds[soundID] = pSound;

	return {this, soundID};
}

void Arg::Sound::SoundEngine::SetListenerPosition(
	const Vec3& position,
	const Vec3& lookDirection,
	const Vec3& upDirection
) const
{
	const irrklang::vec3df listenerPosition(position.x, -position.y, position.z);
	const irrklang::vec3df listenerLookDirection(lookDirection.x, -lookDirection.y, lookDirection.z);
	const irrklang::vec3df listenerVelocity(0.0f, 0.0f, 0.0f);
	const irrklang::vec3df worldUp(upDirection.x, -upDirection.y, upDirection.z);

	m_pEngine->setListenerPosition(
		listenerPosition,
		listenerLookDirection,
		listenerVelocity,
		worldUp
	);
}

void Arg::Sound::SoundEngine::StopAllSounds()
{
	for (const auto& pSound : m_Sounds | std::views::values)
	{
		pSound->drop();
	}

	m_Sounds.clear();
	m_pEngine->removeAllSoundSources();
}

auto Arg::Sound::SoundEngine::GetSound(const GUID& ID) const -> irrklang::ISound*
{
	if (!m_Sounds.contains(ID))
	{
		return nullptr;
	}

	return m_Sounds.at(ID);
}
