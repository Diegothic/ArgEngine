#include <arg_pch.hpp>
#include "SkeletonResource.hpp"

Arg::Content::SkeletonResource::SkeletonResource(const std::shared_ptr<Resource>& resource)
	: GameDataResource(resource)
{
}

auto Arg::Content::SkeletonResource::GetResourceFileExtension() const -> const std::string&
{
	return SKELETON_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::SkeletonResource::VPreLoad()
{
	GameDataResource::VPreLoad();
	m_pSkeletonSpec = std::make_unique<Renderer::SkeletonSpec>();
	m_pSkeleton = std::make_unique<Renderer::Skeleton>();
}

void Arg::Content::SkeletonResource::VOnLoad()
{
	GameDataResource::VOnLoad();
	const auto data = GetData()->data();
	Renderer::SkeletonData skeletonData{
		.Spec = *m_pSkeletonSpec,
		.Bones = std::vector<Renderer::Bone>(m_pSkeletonSpec->BoneCount),
	};

	std::stringstream stream;
	stream.write((char*)data, GetData()->size() * sizeof(char));
	stream.read(
		(char*)skeletonData.Bones.data(),
		m_pSkeletonSpec->BoneCount * sizeof(Renderer::Bone)
	);

	m_pSkeleton->SetData(skeletonData);
}

auto Arg::Content::SkeletonResource::VGetSerializableData() const -> ISerializable*
{
	return m_pSkeletonSpec.get();
}

void Arg::Content::SkeletonResource::VUnload()
{
	m_pSkeleton = nullptr;
	m_pSkeletonSpec = nullptr;
}
