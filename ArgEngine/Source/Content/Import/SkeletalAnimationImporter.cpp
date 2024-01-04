#include <arg_pch.hpp>
#include "SkeletalAnimationImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"
#include "ImporterDataConversion.hpp"
#include "Content/Resource/GameResources/SkeletalAnimationResource.hpp"
#include "Debug/Assert.hpp"

void Arg::Import::SkeletalAnimationImporter::ScanFile(const std::string& file)
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(file, aiProcess_LimitBoneWeights);

	m_AnimationCount = 0;
	m_AnimationsMeshIndex = -1;
	m_AnimationNames.clear();

	if (!scene
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode
		|| scene->mNumMeshes < 1
		|| scene->mNumAnimations < 1
	)
	{
		return;
	}

	m_AnimationsMeshIndex = -1;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		if (scene->mMeshes[i]->mNumBones > 0)
		{
			m_AnimationsMeshIndex = static_cast<int32_t>(i);
			break;
		}
	}
	if (m_AnimationsMeshIndex < 0)
	{
		return;
	}

	m_AnimationCount = scene->mNumAnimations;
	m_AnimationNames.clear();
	for (size_t i = 0; i < m_AnimationCount; i++)
	{
		const auto animation = scene->mAnimations[i];
		std::string animationName = DataConversion::Convert(animation->mName);
		std::string::size_type pos;
		while ((pos = animationName.find('|')) != std::string::npos)
		{
			animationName = animationName.replace(pos, 1, 1, '_');
		}

		m_AnimationNames.push_back(animationName);
	}
}

void Arg::Import::SkeletalAnimationImporter::SetIndex(size_t index)
{
	ARG_ASSERT(index < m_AnimationCount, "");
	m_CurrentIndex = index;
}

auto Arg::Import::SkeletalAnimationImporter::GetAnimationName(size_t index) -> const std::string&
{
	ARG_ASSERT(index < m_AnimationCount, "");
	return m_AnimationNames[index];
}

auto Arg::Import::SkeletalAnimationImporter::ImportFile(const std::string& file) -> bool
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(file, aiProcess_LimitBoneWeights);

	if (!scene
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode
		|| scene->mNumMeshes < 1
		|| scene->mNumAnimations < 1
		|| m_AnimationsMeshIndex < 0
	)
	{
		ARG_CONSOLE_LOG_ERR("Failed to import skeletal animation from \"%s\"!\nReason: %s",
		                    file.c_str(),
		                    importer.GetErrorString()
		);
		return false;
	}

	const auto mesh = scene->mMeshes[m_AnimationsMeshIndex];
	std::unordered_map<std::string, int32_t> boneMap(mesh->mNumBones);
	for (uint32_t i = 0; i < mesh->mNumBones; i++)
	{
		const auto importedBone = mesh->mBones[i];
		const std::string boneName = DataConversion::Convert(importedBone->mName);
		boneMap[boneName] = i;
	}

	const auto animation = scene->mAnimations[m_CurrentIndex];

	const std::string name = m_AnimationNames[m_CurrentIndex];
	const float duration = animation->mDuration;
	const float ticksPerSecond = animation->mTicksPerSecond;

	m_Data.Spec.Name = name;
	m_Data.Spec.Duration = duration;
	m_Data.Spec.TicksPerSecond = ticksPerSecond;

	m_Data.Spec.ChannelCount = mesh->mNumBones;
	m_Data.Spec.ChannelPositionKeysCount = std::vector<size_t>(mesh->mNumBones);
	m_Data.Spec.ChannelRotationKeysCount = std::vector<size_t>(mesh->mNumBones);
	m_Data.Spec.ChannelScaleKeysCount = std::vector<size_t>(mesh->mNumBones);

	m_Data.Channels = std::vector<Renderer::SkeletalAnimationChannel>(mesh->mNumBones);

	for (uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		const auto channel = animation->mChannels[i];

		const std::string channelName = DataConversion::Convert(channel->mNodeName);
		if (!boneMap.contains(channelName))
		{
			continue;
		}

		const int32_t boneIndex = boneMap[channelName];

		m_Data.Channels[boneIndex].BoneIndex = boneIndex;

		m_Data.Spec.ChannelPositionKeysCount[boneIndex] = channel->mNumPositionKeys;
		m_Data.Spec.ChannelRotationKeysCount[boneIndex] = channel->mNumRotationKeys;
		m_Data.Spec.ChannelScaleKeysCount[boneIndex] = channel->mNumScalingKeys;

		m_Data.Channels[boneIndex].PositionKeys
			= std::vector<Renderer::SkeletalAnimationPositionKey>(channel->mNumPositionKeys);
		m_Data.Channels[boneIndex].RotationKeys
			= std::vector<Renderer::SkeletalAnimationRotationKey>(channel->mNumRotationKeys);
		m_Data.Channels[boneIndex].ScaleKeys
			= std::vector<Renderer::SkeletalAnimationScaleKey>(channel->mNumScalingKeys);

		for (size_t j = 0; j < channel->mNumPositionKeys; j++)
		{
			const auto positionKey = channel->mPositionKeys[j];
			const Renderer::SkeletalAnimationPositionKey key{
				.Time = static_cast<float>(positionKey.mTime),
				.Value = DataConversion::Convert(positionKey.mValue)
			};

			m_Data.Channels[boneIndex].PositionKeys[j] = key;
		}

		for (size_t j = 0; j < channel->mNumRotationKeys; j++)
		{
			const auto rotationKey = channel->mRotationKeys[j];
			const Renderer::SkeletalAnimationRotationKey key{
				.Time = static_cast<float>(rotationKey.mTime),
				.Value = DataConversion::Convert(rotationKey.mValue)
			};

			m_Data.Channels[boneIndex].RotationKeys[j] = key;
		}

		for (size_t j = 0; j < channel->mNumScalingKeys; j++)
		{
			const auto scaleKey = channel->mScalingKeys[j];
			const Renderer::SkeletalAnimationScaleKey key{
				.Time = static_cast<float>(scaleKey.mTime),
				.Value = DataConversion::Convert(scaleKey.mValue)
			};

			m_Data.Channels[boneIndex].ScaleKeys[j] = key;
		}
	}

	return true;
}

void Arg::Import::SkeletalAnimationImporter::Save(
	const std::string& fileName,
	const std::filesystem::path& destination
)
{
	auto resourceFile = destination / fileName;
	resourceFile.replace_extension(Content::SKELETAL_ANIMATION_RESOURCE_FILE_EXTENSION);
	auto dataFile = destination / fileName;
	dataFile.replace_extension(Content::RESOURCE_DATA_FILE_EXTENSION);

	m_Data.Spec.Serialize(resourceFile);

	std::ofstream fileStream(
		dataFile,
		std::ios::out
		| std::ios::binary
		| std::ofstream::trunc
	);

	for (size_t i = 0; i < m_Data.Spec.ChannelCount; i++)
	{
		fileStream.write(
			(char*)m_Data.Channels[i].PositionKeys.data(),
			m_Data.Spec.ChannelPositionKeysCount[i] * sizeof(Renderer::SkeletalAnimationPositionKey)
		);
		fileStream.write(
			(char*)m_Data.Channels[i].RotationKeys.data(),
			m_Data.Spec.ChannelRotationKeysCount[i] * sizeof(Renderer::SkeletalAnimationRotationKey)
		);
		fileStream.write(
			(char*)m_Data.Channels[i].ScaleKeys.data(),
			m_Data.Spec.ChannelScaleKeysCount[i] * sizeof(Renderer::SkeletalAnimationScaleKey)
		);
	}

	fileStream.close();
}

auto Arg::Import::SkeletalAnimationImporter::GetData() const -> const Renderer::SkeletalAnimationData&
{
	return m_Data;
}
