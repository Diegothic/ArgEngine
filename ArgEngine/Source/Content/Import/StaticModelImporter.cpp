#include <arg_pch.hpp>
#include "StaticModelImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"
#include "Content/Resource/GameResources/StaticModelResource.hpp"

auto Arg::Import::StaticModelImporter::ImportFile(const std::string& file) -> bool
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
		ARG_CONSOLE_LOG_ERR("Failed to import static mesh \"%s\"!\nReason: %s",
			file.c_str(),
			importer.GetErrorString()
		);
		return false;
	}

	m_Data.Meshes.clear();
	m_Data.Spec.MaterialIndices.clear();

	m_Data.Spec.MeshCount = scene->mNumMeshes;
	m_Data.Meshes = std::vector<Renderer::StaticMeshData>(scene->mNumMeshes);
	m_Data.Spec.MaterialIndices = std::vector<int32_t>(scene->mNumMeshes);
	m_Data.Spec.VertexCounts = std::vector<size_t>(scene->mNumMeshes);
	m_Data.Spec.IndexCounts = std::vector<size_t>(scene->mNumMeshes);

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		const auto mesh = scene->mMeshes[i];
		m_Data.Spec.MaterialIndices[i] = mesh->mMaterialIndex;

		m_Data.Spec.VertexCounts[i] = mesh->mNumVertices;
		m_Data.Spec.IndexCounts[i] = mesh->mNumFaces * 3;
		m_Data.Meshes[i].Vertices = std::vector<Renderer::StaticVertex>(mesh->mNumVertices);
		m_Data.Meshes[i].Indices = std::vector<uint32_t>(mesh->mNumFaces * 3);
		for (uint32_t j = 0; j < mesh->mNumVertices; j++)
		{
			Vec3 position(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			Vec3 normal(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			Vec2 uv(0.0f);
			if (mesh->mTextureCoords[0])
			{
				uv.x = mesh->mTextureCoords[0][j].x;
				uv.y = mesh->mTextureCoords[0][j].y;
			}
			m_Data.Meshes[i].Vertices[j] = { position, normal, uv };
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

void Arg::Import::StaticModelImporter::Save(
	const std::string& fileName,
	const std::filesystem::path& destination
)
{
	auto resourceFile = destination / fileName;
	resourceFile.replace_extension(Content::STATIC_MODEL_RESOURCE_FILE_EXTENSION);
	auto dataFile = destination / fileName;
	dataFile.replace_extension(Content::RESOURCE_DATA_FILE_EXTENSION);

	m_Data.Spec.Serialize(resourceFile);

	std::ofstream fileStream(dataFile,
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
			vertexCount * sizeof(Renderer::StaticVertex)
		);
		fileStream.write(
			(char*)m_Data.Meshes[i].Indices.data(),
			indexCount * sizeof(uint32_t)
		);
	}

	fileStream.close();
}

auto Arg::Import::StaticModelImporter::GetData() const -> const Renderer::StaticModelData&
{
	return m_Data;
}
