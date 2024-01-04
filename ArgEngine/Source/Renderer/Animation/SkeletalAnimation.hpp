#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct SkeletalAnimationSpec : public Content::YamlSerializable
		{
			std::string Name;
			float Duration = 0.0f;
			float TicksPerSecond = 24.0f;
			size_t ChannelCount;
			std::vector<size_t> ChannelPositionKeysCount;
			std::vector<size_t> ChannelRotationKeysCount;
			std::vector<size_t> ChannelScaleKeysCount;

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

			auto GetDuration() const -> float { return m_Spec.Duration; }
			void CalculateTransforms(
				float time,
				bool bLooping,
				std::vector<Mat4>& outBoneTransforms
			) const;

			void SetData(const SkeletalAnimationData& data);

		private:
			SkeletalAnimationSpec m_Spec;
			std::vector<SkeletalAnimationChannel> m_Channels;
		};
	}
}
