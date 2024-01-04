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
	m_pAnimationSpec = std::make_unique<Renderer::SkeletalAnimationSpec>();
	m_pAnimation = std::make_unique<Renderer::SkeletalAnimation>();
}

void Arg::Content::SkeletalAnimationResource::VOnLoad()
{
	GameDataResource::VOnLoad();
	const auto data = GetData()->data();
	Renderer::SkeletalAnimationData animationData{
		.Spec = *m_pAnimationSpec,
		.Channels = std::vector<Renderer::SkeletalAnimationChannel>(m_pAnimationSpec->ChannelCount),
	};

	std::stringstream stream;
	stream.write((char*)data, GetData()->size() * sizeof(char));

	for (size_t i = 0; i < m_pAnimationSpec->ChannelCount; i++)
	{
		animationData.Channels[i].BoneIndex = static_cast<int32_t>(i);
		animationData.Channels[i].PositionKeys = std::vector<Renderer::SkeletalAnimationPositionKey>(
			m_pAnimationSpec->ChannelPositionKeysCount[i]
		);
		animationData.Channels[i].RotationKeys = std::vector<Renderer::SkeletalAnimationRotationKey>(
			m_pAnimationSpec->ChannelRotationKeysCount[i]
		);
		animationData.Channels[i].ScaleKeys = std::vector<Renderer::SkeletalAnimationScaleKey>(
			m_pAnimationSpec->ChannelScaleKeysCount[i]
		);

		stream.read(
			(char*)animationData.Channels[i].PositionKeys.data(),
			m_pAnimationSpec->ChannelPositionKeysCount[i] * sizeof(Renderer::SkeletalAnimationPositionKey)
		);
		stream.read(
			(char*)animationData.Channels[i].RotationKeys.data(),
			m_pAnimationSpec->ChannelRotationKeysCount[i] * sizeof(Renderer::SkeletalAnimationRotationKey)
		);
		stream.read(
			(char*)animationData.Channels[i].ScaleKeys.data(),
			m_pAnimationSpec->ChannelScaleKeysCount[i] * sizeof(Renderer::SkeletalAnimationScaleKey)
		);
	}

	m_pAnimation->SetData(animationData);
}

auto Arg::Content::SkeletalAnimationResource::VGetSerializableData() const -> ISerializable*
{
	return m_pAnimationSpec.get();
}

void Arg::Content::SkeletalAnimationResource::VUnload()
{
	m_pAnimation = nullptr;
	m_pAnimationSpec = nullptr;
}
