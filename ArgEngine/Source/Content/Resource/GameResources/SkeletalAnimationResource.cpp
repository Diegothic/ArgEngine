#include <arg_pch.hpp>
#include "SkeletalAnimationResource.hpp"

Arg::Content::SkeletalAnimationResource::SkeletalAnimationResource(const std::shared_ptr<Resource>& resource)
	: GameDataResource(resource)
{
}

auto Arg::Content::SkeletalAnimationResource::GetResourceFileExtension() const -> const std::string&
{
	return SKELETAL_ANIMATION_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::SkeletalAnimationResource::VPreLoad()
{
	GameDataResource::VPreLoad();
	m_pAnimation = std::make_unique<Renderer::SkeletalAnimation>();
}

void Arg::Content::SkeletalAnimationResource::VOnLoad()
{
	GameDataResource::VOnLoad();
	const auto data = GetData()->data();
	const Renderer::SkeletalAnimationSpec& pSpec = m_pAnimation->GetSpec();
	Renderer::SkeletalAnimationData animationData{
		.Spec = pSpec,
		.Channels = std::vector<Renderer::SkeletalAnimationChannel>(pSpec.ChannelCount),
	};

	std::stringstream stream;
	stream.write((char*)data, GetData()->size() * sizeof(char));

	for (size_t i = 0; i < pSpec.ChannelCount; i++)
	{
		animationData.Channels[i].BoneIndex = static_cast<int32_t>(i);
		animationData.Channels[i].PositionKeys = std::vector<Renderer::SkeletalAnimationPositionKey>(
			pSpec.ChannelPositionKeysCount[i]
		);
		animationData.Channels[i].RotationKeys = std::vector<Renderer::SkeletalAnimationRotationKey>(
			pSpec.ChannelRotationKeysCount[i]
		);
		animationData.Channels[i].ScaleKeys = std::vector<Renderer::SkeletalAnimationScaleKey>(
			pSpec.ChannelScaleKeysCount[i]
		);

		stream.read(
			(char*)animationData.Channels[i].PositionKeys.data(),
			pSpec.ChannelPositionKeysCount[i] * sizeof(Renderer::SkeletalAnimationPositionKey)
		);
		stream.read(
			(char*)animationData.Channels[i].RotationKeys.data(),
			pSpec.ChannelRotationKeysCount[i] * sizeof(Renderer::SkeletalAnimationRotationKey)
		);
		stream.read(
			(char*)animationData.Channels[i].ScaleKeys.data(),
			pSpec.ChannelScaleKeysCount[i] * sizeof(Renderer::SkeletalAnimationScaleKey)
		);
	}

	m_pAnimation->SetData(animationData);
}

auto Arg::Content::SkeletalAnimationResource::VGetSerializableData() const -> ISerializable*
{
	return &m_pAnimation->GetSpec();
}

void Arg::Content::SkeletalAnimationResource::VUnload()
{
	m_pAnimation = nullptr;
}
