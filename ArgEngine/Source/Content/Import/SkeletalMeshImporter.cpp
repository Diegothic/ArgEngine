#include <arg_pch.hpp>
#include "SkeletalMeshImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"
#include "ImporterDataConversion.hpp"

auto Arg::Import::SkeletalMeshImporter::ImportFile(const std::string& file) -> bool
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(file,
		aiProcess_Triangulate
		| aiProcess_LimitBoneWeights);

	if (!scene
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode
		|| scene->mNumMeshes < 1
		)
	{
		ARG_CONSOLE_LOG_ERR("Failed to import skeletal mesh \"%s\"!\nReason: %s",
			file.c_str(),
			importer.GetErrorString()
		);
		return false;
	}

	m_Data.Vertices.clear();
	m_Data.Indices.clear();

	for (uint32_t i = 0; i < 1; i++)
	{
		const auto mesh = scene->mMeshes[i];

		std::vector<Vec4i> boneIndices(mesh->mNumVertices);
		std::vector<Vec4> boneWeights(mesh->mNumVertices);
		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			boneIndices[j] = Vec4i(-1);
			boneWeights[j] = Vec4(0.0f);
		}

		for (uint32_t j = 0; j < mesh->mNumBones; j++)
		{
			const auto bone = mesh->mBones[j];
			for (uint32_t k = 0; k < bone->mNumWeights; k++)
			{
				const auto weight = bone->mWeights[k];
				const int32_t vertexIndex = weight.mVertexId;
				const float vertexWeight = weight.mWeight;

				uint32_t weightIndex = 0;
				for (uint32_t l = 0; l < 4; l++)
				{
					if (boneIndices[vertexIndex][l] < 0)
					{
						weightIndex = l;
						break;
					}
				}

				boneIndices[vertexIndex][weightIndex] = j;
				boneWeights[vertexIndex][weightIndex] = vertexWeight;
			}
		}

		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			Renderer::SkeletalVertex vertex{
				.Position = DataConversion::Convert(mesh->mVertices[j]),
				.Normal = DataConversion::Convert(mesh->mNormals[j]),
				.TexUV = DataConversion::Convert(mesh->mTextureCoords[0][j]),
				.BoneIndices = boneIndices[j],
				.BoneWeights = boneWeights[j],
			};

			m_Data.Vertices.push_back(vertex);
		}

		for (uint32_t j = 0; j < mesh->mNumFaces; j++)
		{
			const auto face = &mesh->mFaces[j];
			for (uint32_t k = 0; k < face->mNumIndices; k++)
			{
				m_Data.Indices.push_back(face->mIndices[k]);
			}
		}
	}

	return true;
}

auto Arg::Import::SkeletalMeshImporter::GetData() const -> const Renderer::SkeletalMeshData&
{
	return m_Data;
}
