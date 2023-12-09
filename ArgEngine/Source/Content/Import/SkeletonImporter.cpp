#include <arg_pch.hpp>
#include "SkeletonImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"
#include "ImporterDataConversion.hpp"

auto Arg::Import::SkeletonImporter::ImportFile(const std::string& file) -> bool
{
	Assimp::Importer importer;
	const aiScene* scene = importer.
		ReadFile(file, aiProcess_PopulateArmatureData);

	if (!scene
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode
		|| scene->mNumMeshes < 1
		)
	{
		ARG_CONSOLE_LOG_ERR("Failed to import skeleton \"%s\"!\nReason: %s",
			file.c_str(),
			importer.GetErrorString()
		);
		return false;
	}

	m_Data.Bones.clear();
	m_Data.BoneMap.clear();

	int32_t meshIndex = -1;
	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		if (scene->mMeshes[i]->mNumBones > 0)
		{
			meshIndex = i;
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
	for (uint32_t i = 0; i < mesh->mNumBones; i++)
	{
		const auto importedBone = mesh->mBones[i];
		const std::string boneName = DataConversion::Convert(importedBone->mName);
		const std::string parentName = DataConversion::Convert(
			importedBone->mNode->mParent->mName
		);
		uint32_t parentIndex = -1;
		if (i > 0)
		{
			parentIndex = m_Data.Bones[m_Data.BoneMap[parentName]].Index;
		}

		Renderer::Bone bone{
			.Index = static_cast<int32_t>(i),
			.ParentIndex = static_cast<int32_t>(parentIndex),
			.Name = boneName,
			.LocalTransform = DataConversion::Convert(importedBone->mNode->mTransformation),
			.OffsetTransform = DataConversion::Convert(importedBone->mOffsetMatrix)
		};

		m_Data.BoneMap[boneName] = i;
		m_Data.Bones.push_back(bone);
	}

	return true;
}

auto Arg::Import::SkeletonImporter::GetData() const -> const Renderer::SkeletonSpec&
{
	return m_Data;
}
