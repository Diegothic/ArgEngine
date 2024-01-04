#include <arg_pch.hpp>
#include "SkeletalAnimation.hpp"

void Arg::Renderer::SkeletalAnimation::CalculateTransforms(
	float time,
	bool bLooping,
	std::vector<Mat4>& outBoneTransforms
) const
{
	outBoneTransforms.clear();

	const float timeElapsed = time * m_Spec.TicksPerSecond;
	const float animationTime = (!bLooping && timeElapsed >= m_Spec.Duration)
		                            ? m_Spec.Duration
		                            : timeElapsed - (static_cast<int32_t>(timeElapsed / m_Spec.Duration)
			                            * m_Spec.Duration);
	for (auto i = 0; i < m_Spec.ChannelCount; i++)
	{
		outBoneTransforms.emplace_back(1.0f);
		const auto boneChannels = m_Channels[i];
		{
			int32_t keyIndex = -1;
			for (auto j = 0; j < boneChannels.PositionKeys.size(); j++)
			{
				if (boneChannels.PositionKeys[j].Time >= animationTime)
				{
					keyIndex = j;
					break;
				}
			}

			if (keyIndex >= 0)
			{
				const int32_t prevKeyIndex = keyIndex > 0 ? keyIndex - 1 : 0;
				const float keyTime = boneChannels
					.PositionKeys[keyIndex].Time;
				const float prevKeyTime = boneChannels
					.PositionKeys[prevKeyIndex].Time;
				const float keyDiff = keyTime - prevKeyTime;
				float percent = (animationTime - prevKeyTime);
				if (keyDiff > 0.0f)
				{
					percent /= keyDiff;
				}
				const Vec3 position = (percent
						* boneChannels.PositionKeys[keyIndex].Value)
					+ ((1.0f - percent)
						* boneChannels.PositionKeys[prevKeyIndex].Value);

				outBoneTransforms[i] = Math::translate(
					outBoneTransforms[i],
					position
				);
			}
		}
		{
			int32_t keyIndex = -1;
			for (auto j = 0; j < boneChannels.RotationKeys.size(); j++)
			{
				if (boneChannels.RotationKeys[j].Time >= animationTime)
				{
					keyIndex = j;
					break;
				}
			}

			if (keyIndex >= 0)
			{
				const int32_t prevKeyIndex = keyIndex > 0 ? keyIndex - 1 : 0;
				const float keyTime = boneChannels
					.RotationKeys[keyIndex].Time;
				const float prevKeyTime = boneChannels
					.RotationKeys[prevKeyIndex].Time;
				const float keyDiff = keyTime - prevKeyTime;
				float percent = (animationTime - prevKeyTime);
				if (keyDiff > 0.0f)
				{
					percent /= keyDiff;
				}
				const Quat rotation = Math::slerp(
					boneChannels
					.RotationKeys[prevKeyIndex].Value,
					boneChannels
					.RotationKeys[keyIndex].Value,
					percent);

				const Mat4 rotationMat = Math::mat4_cast(rotation);
				outBoneTransforms[i] = outBoneTransforms[i]
					* rotationMat;
			}
		}
		{
			int32_t keyIndex = -1;
			for (auto j = 0; j < boneChannels.ScaleKeys.size(); j++)
			{
				if (boneChannels.ScaleKeys[j].Time >= animationTime)
				{
					keyIndex = j;
					break;
				}
			}

			if (keyIndex >= 0)
			{
				const int32_t prevKeyIndex = keyIndex > 0 ? keyIndex - 1 : 0;
				const float keyTime = boneChannels
					.ScaleKeys[keyIndex].Time;
				const float prevKeyTime = boneChannels
					.ScaleKeys[prevKeyIndex].Time;
				const float keyDiff = keyTime - prevKeyTime;
				float percent = (animationTime - prevKeyTime);
				if (keyDiff > 0.0f)
				{
					percent /= keyDiff;
				}
				const Vec3 scale = (percent
						* boneChannels.ScaleKeys[keyIndex].Value)
					+ ((1.0f - percent)
						* boneChannels.ScaleKeys[prevKeyIndex].Value);

				outBoneTransforms[i] = Math::scale(
					outBoneTransforms[i],
					scale
				);
			}
		}
	}
}

void Arg::Renderer::SkeletalAnimation::SetData(const SkeletalAnimationData& data)
{
	m_Channels.clear();
	m_Spec = data.Spec;
	m_Channels = data.Channels;
}

auto Arg::Renderer::SkeletalAnimationSpec::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["SkeletalAnimation"];
	header["Name"] = Name;
	header["Duration"] = Duration;
	header["TicksPerSecond"] = TicksPerSecond;
	header["ChannelCount"] = ChannelCount;

	auto channelCountsNode = header["ChannelCounts"];
	for (size_t i = 0; i < ChannelCount; i++)
	{
		channelCountsNode["Position"][i] = ChannelPositionKeysCount[i];
		channelCountsNode["Rotation"][i] = ChannelRotationKeysCount[i];
		channelCountsNode["Scale"][i] = ChannelScaleKeysCount[i];
	}

	header["ChannelCounts"] = channelCountsNode;
	node["SkeletalAnimation"] = header;

	return true;
}

auto Arg::Renderer::SkeletalAnimationSpec::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto header = node["SkeletalAnimation"];
	if (!header)
	{
		return false;
	}

	Name = ValueOr<std::string>(header["Name"], "Null");
	Duration = ValueOr<float>(header["Duration"], 0.0f);
	TicksPerSecond = ValueOr<float>(header["TicksPerSecond"], 24.0f);
	ChannelCount = ValueOr<size_t>(header["ChannelCount"], 0);
	if (header["ChannelCounts"])
	{
		const auto channelCountsNode = header["ChannelCounts"];

		ChannelPositionKeysCount = std::vector<size_t>(ChannelCount);
		for (size_t i = 0; i < ChannelCount; i++)
		{
			ChannelPositionKeysCount[i] = ValueOr<size_t>(channelCountsNode["Position"][i], 0);
		}

		ChannelRotationKeysCount = std::vector<size_t>(ChannelCount);
		for (size_t i = 0; i < ChannelCount; i++)
		{
			ChannelRotationKeysCount[i] = ValueOr<size_t>(channelCountsNode["Rotation"][i], 0);
		}

		ChannelScaleKeysCount = std::vector<size_t>(ChannelCount);
		for (size_t i = 0; i < ChannelCount; i++)
		{
			ChannelScaleKeysCount[i] = ValueOr<size_t>(channelCountsNode["Scale"][i], 0);
		}
	}

	return true;
}
