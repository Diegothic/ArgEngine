#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/GameResources/SoundResource.hpp"
#include "Gameplay/World/Actor/Component/ActorComponent.hpp"
#include "Sound/SoundEngine.hpp"

namespace Arg
{
	using SoundResourceHandle = Content::ResourceHandle<Content::SoundResource>;

	namespace Gameplay
	{
		class SoundPlayerComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			SoundPlayerComponent();

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override { return COMPONENT_ID; }
			auto VGetName() const -> const std::string& override { return COMPONENT_NAME; }

			void VBeginPlay() override;
			void VTick(const GameTime& gameTime) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

		public:
			void Play();
			void Pause();
			void Unpause();
			void Stop();
			auto IsPlaying() const -> bool;

		public:
			auto GetSound() const -> SoundResourceHandle;
			void SetSound(SoundResourceHandle soundHandle);
			
			auto GetPlayOnStart() const -> bool;
			void SetPlayOnStart(bool bPlayOnStart);
			
			auto GetVolume() const -> float;
			void SetVolume(float volume);

			auto GetVolumeVariance() const -> float;
			void SetVolumeVariance(float variance);

			auto GetPitch() const -> float;
			void SetPitch(float pitch);

			auto GetPitchVariance() const -> float;
			void SetPitchVariance(float variance);

			auto GetIsLooping() const -> bool;
			void SetIsLooping(bool bIsLooping);

			auto GetIs3D() const -> bool;
			void SetIs3D(bool bIs3D);

			auto GetDistance() const -> float;
			void SetDistance(float distance);

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			Sound::SoundSpec m_Spec;
			Sound::SoundHandle m_SoundHandle;
			bool m_bPlayOnStart = true;
			bool m_bIs3D = true;
			SoundResourceHandle m_SoundResource;
		};
	}
}
