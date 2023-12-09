#include <arg_pch.hpp>
#include "SkeletalAnimationImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"
#include "ImporterDataConversion.hpp"

auto Arg::Import::SkeletalAnimationImporter::ImportFile(const std::string& file) -> bool
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(file, aiProcess_LimitBoneWeights);

	if (!scene
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode
		|| scene->mNumMeshes < 1
		|| scene->mNumAnimations < 1
		)
	{
		ARG_CONSOLE_LOG_ERR("Failed to import skeletal animation from \"%s\"!\nReason: %s",
			file.c_str(),
			importer.GetErrorString()
		);
		return false;
	}

	int32_t meshIndex = -1;
	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		if (scene->mMeshes[i]->mNumBones > 0)
		{
			meshIndex = static_cast<int32_t>(i);
			break;
		}
	}
	if (meshIndex < 0)
	{
		ARG_CONSOLE_LOG_ERR("Skeleton not found in \"%s\"!",
			file.c_str()
		);
		return false;
	}


	const auto mesh = scene->mMeshes[meshIndex];
	std::unordered_map<std::string, int32_t> boneMap(mesh->mNumBones);
	for (uint32_t i = 0; i < mesh->mNumBones; i++)
	{
		const auto importedBone = mesh->mBones[i];
		const std::string boneName = DataConversion::Convert(importedBone->mName);
		boneMap[boneName] = i;
	}

	for (uint32_t i = 2; i < 3; i++)
	{
		const auto animation = scene->mAnimations[i];

		const std::string name = DataConversion::Convert(animation->mName);
		const float duration = animation->mDuration;
		const float ticksPerSecond = animation->mTicksPerSecond;

		m_Data.Name = name;
		m_Data.Duration = duration;
		m_Data.TicksPerSecond = ticksPerSecond;
		m_Data.Channels = std::vector<Renderer::SkleletalAnimationChannel>(mesh->mNumBones);

		for (uint32_t j = 0; j < animation->mNumChannels; j++)
		{
			const auto channel = animation->mChannels[j];
			const std::string channelName = DataConversion::Convert(channel->mNodeName);
			if (!boneMap.contains(channelName))
			{
				continue;
			}

			const int32_t boneIndex = boneMap[channelName];

			m_Data.Channels[boneIndex].BoneIndex = boneIndex;
			m_Data.Channels[boneIndex].PositionKeys
				= std::vector<Renderer::SkeletalAnimationPositionKey>(channel->mNumPositionKeys);
			m_Data.Channels[boneIndex].RotationKeys
				= std::vector<Renderer::SkeletalAnimationRotationKey>(channel->mNumRotationKeys);
			m_Data.Channels[boneIndex].ScaleKeys
				= std::vector<Renderer::SkeletalAnimationScaleKey>(channel->mNumScalingKeys);

			for (uint32_t k = 0; k < channel->mNumPositionKeys; k++)
			{
				const auto positionKey = channel->mPositionKeys[k];
				const Renderer::SkeletalAnimationPositionKey key{
					.Time = static_cast<float>(positionKey.mTime),
					.Value = DataConversion::Convert(positionKey.mValue)
				};

				m_Data.Channels[boneIndex].PositionKeys[k] = key;
			}

			for (uint32_t k = 0; k < channel->mNumRotationKeys; k++)
			{
				const auto rotationKey = channel->mRotationKeys[k];
				const Renderer::SkeletalAnimationRotationKey key{
					.Time = static_cast<float>(rotationKey.mTime),
					.Value = DataConversion::Convert(rotationKey.mValue)
				};

				m_Data.Channels[boneIndex].RotationKeys[k] = key;
			}

			for (uint32_t k = 0; k < channel->mNumScalingKeys; k++)
			{
				const auto scaleKey = channel->mScalingKeys[k];
				const Renderer::SkeletalAnimationScaleKey key{
					.Time = static_cast<float>(scaleKey.mTime),
					.Value = DataConversion::Convert(scaleKey.mValue)
				};

				m_Data.Channels[boneIndex].ScaleKeys[k] = key;
			}
		}
	}

	return true;
}

auto Arg::Import::SkeletalAnimationImporter::GetData() const -> const Renderer::SkeletalAnimationData&
{
	return m_Data;
}
