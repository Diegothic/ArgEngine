#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct Bone
		{
			int32_t Index = 0;
			int32_t ParentIndex = -1;
			std::string Name = std::string();
			Mat4 LocalTransform = Mat4(1.0f);
			Mat4 OffsetTransform = Mat4(1.0f);
		};

		struct SkeletonSpec
		{
			std::vector<Bone> Bones;
			std::unordered_map<std::string, int32_t> BoneMap;
		};

		class Skeleton
		{
		public:
			Skeleton(const SkeletonSpec& spec) {};
			~Skeleton() = default;

			auto CalculatePose(
				const std::vector<const Mat4>& boneTransforms
			) -> std::vector<Mat4> {};

		private:
			int32_t m_BoneCount;
			std::vector<Bone> m_Bones;
		};
	}
}
