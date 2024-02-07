#include <arg_pch.hpp>
#include "Skeleton.hpp"

#include "Debug/Assert.hpp"

void Arg::Renderer::SkeletonPose::Apply(ShaderProgram* pShader) const
{
	for (auto i = 0; i < BoneTransforms.size(); i++)
	{
		pShader->SetUniform(
			std::format("u_BoneTransforms[{}]", i),
			BoneTransforms[i]
		);
	}
}

auto Arg::Renderer::Skeleton::GetBoneCount() const -> size_t
{
	return m_Bones.size();
}

auto Arg::Renderer::Skeleton::GetBoneName(size_t boneIndex) const -> const std::string&
{
	ARG_ASSERT(boneIndex < m_Bones.size());
	return m_Spec.BoneNames[boneIndex];
}

auto Arg::Renderer::Skeleton::FindWorldBoneTransform(size_t boneIndex, const SkeletonPose& pose) const -> Mat4
{
	ARG_ASSERT(boneIndex < m_Bones.size());
	return pose.BoneTransforms[boneIndex] * Math::inverse(m_Bones[boneIndex].OffsetTransform);
}

void Arg::Renderer::Skeleton::CalculateRestPose(
	SkeletonPose& outPose
) const
{
	outPose.BoneTransforms.clear();
	for (auto i = 0; i < m_Bones.size(); i++)
	{
		Mat4 boneTransform = m_Bones[i].LocalTransform;
		if (m_Bones[i].ParentIndex > 0)
		{
			boneTransform = outPose.BoneTransforms[m_Bones[i].ParentIndex]
				* boneTransform;
		}

		outPose.BoneTransforms.push_back(boneTransform);
	}

	for (auto i = 0; i < m_Bones.size(); i++)
	{
		outPose.BoneTransforms[i] = outPose.BoneTransforms[i]
			* m_Bones[i].OffsetTransform;
	}
}

void Arg::Renderer::Skeleton::CalculatePose(
	const std::vector<Mat4>& boneTransforms,
	SkeletonPose& outPose
) const
{
	outPose.BoneTransforms.clear();
	for (auto i = 0; i < m_Bones.size(); i++)
	{
		Mat4 boneTransform = boneTransforms[i];
		if (m_Bones[i].ParentIndex > 0)
		{
			boneTransform = outPose.BoneTransforms[m_Bones[i].ParentIndex]
				* boneTransform;
		}

		outPose.BoneTransforms.push_back(boneTransform);
	}

	for (auto i = 0; i < m_Bones.size(); i++)
	{
		outPose.BoneTransforms[i] = outPose.BoneTransforms[i]
			* m_Bones[i].OffsetTransform;
	}
}

void Arg::Renderer::Skeleton::SetData(const SkeletonData& data)
{
	m_Bones.clear();
	m_Spec = data.Spec;
	m_Bones = data.Bones;
}

auto Arg::Renderer::SkeletonSpec::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["Skeleton"];
	header["BoneCount"] = BoneCount;
	for (size_t i = 0; i < BoneCount; i++)
	{
		header["BoneNames"][i] = BoneNames[i];
	}

	node["Skeleton"] = header;

	return true;
}

auto Arg::Renderer::SkeletonSpec::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto header = node["Skeleton"];
	if (!header)
	{
		return false;
	}

	BoneCount = ValueOr<size_t>(header["BoneCount"], 0);
	if (header["BoneNames"])
	{
		BoneNames = std::vector<std::string>(BoneCount);
		for (size_t i = 0; i < BoneCount; i++)
		{
			BoneNames[i] = ValueOr<std::string>(header["BoneNames"][i], "Null");
		}
	}

	return true;
}
