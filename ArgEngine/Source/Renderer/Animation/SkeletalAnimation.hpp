#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct SkeletalAnimationEvent
		{
			int32_t Frame;
			std::string Name;
		};

		struct SkeletalAnimationSpec : public Content::YamlSerializable
		{
			std::string Name;
			float Duration = 0.0f;
			float TicksPerSecond = 24.0f;
			size_t ChannelCount;
			std::vector<size_t> ChannelPositionKeysCount;
			std::vector<size_t> ChannelRotationKeysCount;
			std::vector<size_t> ChannelScaleKeysCount;
			std::vector<SkeletalAnimationEvent> Events;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		struct SkeletalAnimationPositionKey
		{
			float Time = 0.0f;
			Vec3 Value = Vec3(0.0f);
		};

		struct SkeletalAnimationRotationKey
		{
			float Time = 0.0f;
			Quat Value = Quat(Vec3(0.0f));
		};

		struct SkeletalAnimationScaleKey
		{
			float Time = 0.0f;
			Vec3 Value = Vec3(1.0f);
		};

		struct SkeletalAnimationChannel
		{
			int32_t BoneIndex = -1;
			std::vector<SkeletalAnimationPositionKey> PositionKeys;
			std::vector<SkeletalAnimationRotationKey> RotationKeys;
			std::vector<SkeletalAnimationScaleKey> ScaleKeys;
		};

		struct SkeletalAnimationData
		{
			SkeletalAnimationSpec Spec;
			std::vector<SkeletalAnimationChannel> Channels;
		};

		class SkeletalAnimation
		{
		public:
			SkeletalAnimation() = default;
			SkeletalAnimation(const SkeletalAnimation&) = delete;
			~SkeletalAnimation() = default;

			void SetData(const SkeletalAnimationData& data);

		public:
			auto GetDuration() const -> float { return m_Spec.Duration; }
			auto GetTicksPerSecond() const -> float { return m_Spec.TicksPerSecond; }
			auto GetSpec() -> SkeletalAnimationSpec& { return m_Spec; }

			auto GetEventsCount() const -> size_t { return m_Spec.Events.size(); }
			auto GetEvent(size_t index) const -> const SkeletalAnimationEvent&;
			void SetEvent(size_t index, const SkeletalAnimationEvent& event);
			void AddEvent(const SkeletalAnimationEvent& event);
			void RemoveEvent(size_t index);

		public:
			void CalculateTransforms(
				float time,
				bool bLooping,
				std::vector<Mat4>& outBoneTransforms
			) const;

		private:
			SkeletalAnimationSpec m_Spec;
			std::vector<SkeletalAnimationChannel> m_Channels;
		};
	}
}
