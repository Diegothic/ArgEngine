#pragma once

#include <arg_pch.hpp>

#include <irrKlang/irrKlang.h>

#include "Content/Content.hpp"
#include "Content/Resource/GameResources/SoundResource.hpp"
#include "Core/GUID.hpp"
#include "Core/Math/Math.hpp"

namespace Arg
{
	using SoundResourceHandle = Content::ResourceHandle<Content::SoundResource>;

	namespace Sound
	{
		struct SoundSpec
		{
			float Volume = 1.0f;
			float VolumeVariance = 0.0f;
			float Pitch = 1.0f;
			float PitchVariance = 0.0f;
			bool bIsLooping = false;
			Vec3 Position = Vec3(0.0f);
			float Distance = 50.0f;
		};

		class SoundEngine;

		class SoundHandle
		{
		public:
			SoundHandle() = default;
			SoundHandle(SoundEngine* pEngine, const GUID& ID);

		public:
			auto IsValid() const -> bool;

			void Play() const;
			void Pause() const;
			void Stop() const;

			auto IsPlaying() const -> bool;
			void UpdateVolume(float volume) const;
			void UpdatePitch(float pitch) const;
			void UpdatePosition(const Vec3& position) const;
			void UpdateDistance(float distance) const;
			void UpdateLooping(bool value) const;

		private:
			SoundEngine* m_pEngine = nullptr;
			GUID m_ID = GUID::Empty;
		};

		class SoundEngine
		{
		public:
			void Initialize();
			void CleanUp();
			void ClearGarbage();

		public:
			auto PlaySound2D(const SoundResourceHandle& sound, const SoundSpec& spec) -> SoundHandle;
			auto PlaySound3D(const SoundResourceHandle& sound, const SoundSpec& spec) -> SoundHandle;

			void SetListenerPosition(
				const Vec3& position,
				const Vec3& lookDirection,
				const Vec3& upDirection
				) const;
			void StopAllSounds();

		protected:
			auto GetSound(const GUID& ID) const -> irrklang::ISound*;

		private:
			std::unique_ptr<irrklang::ISoundEngine> m_pEngine = nullptr;
			std::unordered_map<GUID, irrklang::ISound*> m_Sounds;
			Random::Random m_Random;

			friend class SoundHandle;
		};
	}
}
