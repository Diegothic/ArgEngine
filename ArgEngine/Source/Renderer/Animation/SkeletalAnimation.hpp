#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Renderer
	{
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

		struct SkleletalAnimationChannel
		{
			int32_t BoneIndex = -1;
			std::vector<SkeletalAnimationPositionKey> PositionKeys;
			std::vector<SkeletalAnimationRotationKey> RotationKeys;
			std::vector<SkeletalAnimationScaleKey> ScaleKeys;
		};

		struct SkeletalAnimationData
		{
			std::string Name = std::string();
			float Duration = 0.0f;
			float TicksPerSecond = 24.0f;
			std::vector<SkleletalAnimationChannel> Channels;
		};

		class SkeletalAnimation
		{

		};
	}
}
