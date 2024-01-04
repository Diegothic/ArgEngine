#include <arg_pch.hpp>
#include "SkeletonImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"
#include "ImporterDataConversion.hpp"
#include "Content/Resource/GameResources/SkeletonResource.hpp"

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
		ARG_CONSOLE_LOG_ERR(
			"Failed to import skeleton \"%s\"!\nReason: %s",
			file.c_str(),
			importer.GetErrorString()
		);
		return false;
	}

	m_Data.Bones.clear();

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
		ARG_CONSOLE_LOG_ERR(
			"Skeleton not found in \"%s\"!",
			file.c_str()
		);
		return false;
	}

	const auto mesh = scene->mMeshes[meshIndex];

	std::unordered_map<std::string, size_t> boneMap;
	boneMap.reserve(mesh->mNumBones);
	m_Data.Spec.BoneCount = mesh->mNumBones;
	m_Data.Spec.BoneNames = std::vector<std::string>(mesh->mNumBones);

	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		const auto importedBone = mesh->mBones[i];
		const std::string boneName = DataConversion::Convert(importedBone->mName);
		const std::string parentName = DataConversion::Convert(
			importedBone->mNode->mParent->mName
		);
		int32_t parentIndex = -1;
		if (i > 0)
		{
			parentIndex = m_Data.Bones[boneMap[parentName]].Index;
		}

		m_Data.Spec.BoneNames[i] = boneName;
		Renderer::Bone bone{
			.Index = static_cast<int32_t>(i),
			.ParentIndex = parentIndex,
			.LocalTransform = DataConversion::Convert(importedBone->mNode->mTransformation),
			.OffsetTransform = DataConversion::Convert(importedBone->mOffsetMatrix)
		};

		boneMap[boneName] = i;
		m_Data.Bones.push_back(bone);
	}

	return true;
}

void Arg::Import::SkeletonImporter::Save(
	const std::string& fileName,
	const std::filesystem::path& destination
)
{
	auto resourceFile = destination / fileName;
	resourceFile.replace_extension(Content::SKELETON_RESOURCE_FILE_EXTENSION);
	auto dataFile = destination / fileName;
	dataFile.replace_extension(Content::RESOURCE_DATA_FILE_EXTENSION);

	m_Data.Spec.Serialize(resourceFile);

	std::ofstream fileStream(
		dataFile,
		std::ios::out
		| std::ios::binary
		| std::ofstream::trunc
	);

	fileStream.write(
		(char*)m_Data.Bones.data(),
		m_Data.Bones.size() * sizeof(Renderer::Bone)
	);

	fileStream.close();
}

auto Arg::Import::SkeletonImporter::GetData() const -> const Renderer::SkeletonData&
{
	return m_Data;
}
