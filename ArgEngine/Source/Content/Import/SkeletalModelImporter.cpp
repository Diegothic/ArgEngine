#include <arg_pch.hpp>
#include "SkeletalModelImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"
#include "ImporterDataConversion.hpp"
#include "Content/Resource/GameResources/SkeletalModelResource.hpp"

auto Arg::Import::SkeletalModelImporter::ImportFile(
	const std::string& file
) -> bool
{
	Assimp::Importer importer;
	const aiScene* scene = importer.
		ReadFile(file, aiProcess_Triangulate);

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

	m_Data.Meshes.clear();
	m_Data.Spec.MaterialIndices.clear();

	m_Data.Spec.MeshCount = scene->mNumMeshes;
	m_Data.Meshes = std::vector<Renderer::SkeletalMeshData>(scene->mNumMeshes);
	m_Data.Spec.MaterialIndices = std::vector<int32_t>(scene->mNumMeshes);
	m_Data.Spec.VertexCounts = std::vector<size_t>(scene->mNumMeshes);
	m_Data.Spec.IndexCounts = std::vector<size_t>(scene->mNumMeshes);

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		const auto mesh = scene->mMeshes[i];
		m_Data.Spec.MaterialIndices[i] = mesh->mMaterialIndex;

		m_Data.Spec.VertexCounts[i] = mesh->mNumVertices;
		m_Data.Spec.IndexCounts[i] = mesh->mNumFaces * 3;
		m_Data.Meshes[i].Vertices = std::vector<Renderer::SkeletalVertex>(mesh->mNumVertices);
		m_Data.Meshes[i].Indices = std::vector<uint32_t>(mesh->mNumFaces * 3);

		std::vector<Vec4i> boneIndices(mesh->mNumVertices);
		std::vector<Vec4> boneWeights(mesh->mNumVertices);
		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			boneIndices[j] = Vec4i(-1);
			boneWeights[j] = Vec4(0.0f);
		}

		if (mesh->mNumBones <= 0)
		{
			ARG_CONSOLE_LOG_ERR("Failed to import skeletal mesh \"%s\"!\nReason: %s",
			                    file.c_str(),
			                    "Found mesh without bones."
			);
			return false;
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
			const Vec3 position = DataConversion::Convert(mesh->mVertices[j]);
			const Vec3 normal = DataConversion::Convert(mesh->mNormals[j]);
			const Vec2 uv = DataConversion::Convert(mesh->mTextureCoords[0][j]);

			m_Data.Meshes[i].Vertices[j] = {position, normal, uv, boneIndices[j], boneWeights[j]};
		}

		for (uint32_t j = 0; j < mesh->mNumFaces; j++)
		{
			const auto face = &mesh->mFaces[j];
			for (uint32_t k = 0; k < 3; k++)
			{
				m_Data.Meshes[i].Indices[j * 3 + k] = face->mIndices[k];
			}
		}
	}

	std::vector<int32_t> materialIndices;
	for (size_t i = 0; i < m_Data.Spec.MaterialIndices.size(); i++)
	{
		bool bFound = false;
		for (size_t j = 0; j < materialIndices.size(); j++)
		{
			if (m_Data.Spec.MaterialIndices[i] == materialIndices[j])
			{
				m_Data.Spec.MaterialIndices[i] = static_cast<int32_t>(j);
				bFound = true;
				break;
			}
		}

		if (bFound)
		{
			continue;
		}

		materialIndices.push_back(m_Data.Spec.MaterialIndices[i]);
		m_Data.Spec.MaterialIndices[i] = static_cast<int32_t>(materialIndices.size()) - 1;
	}

	return true;
}

void Arg::Import::SkeletalModelImporter::Save(
	const std::string& fileName,
	const std::filesystem::path& destination
)
{
	auto resourceFile = destination / fileName;
	resourceFile.replace_extension(Content::SKELETAL_MODEL_RESOURCE_FILE_EXTENSION);
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
		(char*)m_Data.Spec.MaterialIndices.data(),
		m_Data.Spec.MaterialIndices.size() * sizeof(int32_t)
	);

	for (size_t i = 0; i < m_Data.Spec.MeshCount; i++)
	{
		const size_t vertexCount = m_Data.Spec.VertexCounts[i];
		const size_t indexCount = m_Data.Spec.IndexCounts[i];
		fileStream.write(
			(char*)m_Data.Meshes[i].Vertices.data(),
			vertexCount * sizeof(Renderer::SkeletalVertex)
		);
		fileStream.write(
			(char*)m_Data.Meshes[i].Indices.data(),
			indexCount * sizeof(uint32_t)
		);
	}

	fileStream.close();
}

auto Arg::Import::SkeletalModelImporter::GetData() const -> const Renderer::SkeletalModelData&
{
	return m_Data;
}
