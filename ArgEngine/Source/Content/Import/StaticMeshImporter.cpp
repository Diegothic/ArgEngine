#include <arg_pch.hpp>
#include "StaticMeshImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debug/Log.hpp"

auto Arg::Import::StaticMeshImporter::ImportFile(const std::string& file) -> bool
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

	m_Data.Vertices.clear();
	m_Data.Indices.clear();

	for (uint32_t i = 0; i < 1; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

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
			Renderer::StaticVertex vertex{ position, normal, uv };
			m_Data.Vertices.push_back(vertex);
		}

		for (uint32_t j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace* face = &mesh->mFaces[j];
			for (uint32_t k = 0; k < face->mNumIndices; k++)
			{
				m_Data.Indices.push_back(face->mIndices[k]);
			}
		}
	}

	return true;
}

auto Arg::Import::StaticMeshImporter::GetData() const -> const Renderer::StaticMeshData&
{
	return m_Data;
}
