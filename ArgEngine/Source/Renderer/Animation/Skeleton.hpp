#pragma once

#include <arg_pch.hpp>

#include "Content/Serialization/YamlSerializable.hpp"
#include "Core/Math/Math.hpp"
#include "Renderer/ShaderProgram.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct SkeletonPose
		{
			std::vector<Mat4> BoneTransforms;

		public:
			void Apply(ShaderProgram* pShader) const;
		};

		struct SkeletonSpec : public Content::YamlSerializable
		{
			size_t BoneCount;
			std::vector<std::string> BoneNames;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		struct Bone
		{
			int32_t Index = 0;
			int32_t ParentIndex = -1;
			Mat4 LocalTransform = Mat4(1.0f);
			Mat4 OffsetTransform = Mat4(1.0f);
		};

		struct SkeletonData
		{
			SkeletonSpec Spec;
			std::vector<Bone> Bones;
		};

		class Skeleton
		{
		public:
			Skeleton() = default;
			Skeleton(const Skeleton&) = delete;
			~Skeleton() = default;

		public:
			auto GetBoneCount() const -> size_t;
			auto GetBoneName(size_t boneIndex) const -> const std::string&;
			auto FindWorldBoneTransform(size_t boneIndex, const SkeletonPose& pose) const -> Mat4;

		public:
			void CalculateRestPose(
				SkeletonPose& outPose
			) const;
			void CalculatePose(
				const std::vector<Mat4>& boneTransforms,
				SkeletonPose& outPose
			) const;

		public:
			void SetData(const SkeletonData& data);

		private:
			SkeletonSpec m_Spec;
			std::vector<Bone> m_Bones;
		};
	}
}
