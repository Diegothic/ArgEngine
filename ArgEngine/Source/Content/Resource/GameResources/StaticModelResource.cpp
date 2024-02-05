#include <arg_pch.hpp>
#include "StaticModelResource.hpp"

Arg::Content::StaticModelResource::StaticModelResource(const std::shared_ptr<Resource>& resource)
	: GameDataResource(resource)
{
}

auto Arg::Content::StaticModelResource::FindMeshData(size_t meshIndex) -> std::vector<Vec3>
{
	GameDataResource::VPreLoad();

	std::vector<Vec3> result;
	Renderer::StaticModelSpec spec;

	const auto data = GetData()->data();
	std::stringstream stream;
	stream.write((char*)data, GetData()->size() * sizeof(char));
	stream.seekg(m_pModelSpec->MaterialIndices.size() * sizeof(int32_t), std::ios::cur);
	for (size_t i = 0; i < m_pModelSpec->MeshCount; i++)
	{
		const size_t vertexCount = m_pModelSpec->VertexCounts[i];
		const size_t indexCount = m_pModelSpec->IndexCounts[i];
		if (i != meshIndex)
		{
			stream.seekg(vertexCount * sizeof(Renderer::StaticVertex), std::ios::cur);
			stream.seekg(indexCount * sizeof(uint32_t), std::ios::cur);
		}
		else
		{
			std::vector<Renderer::StaticVertex> vertices(vertexCount);
			stream.read(
				(char*)vertices.data(),
				vertexCount * sizeof(Renderer::StaticVertex)
			);

			result.reserve(vertexCount);
			for (size_t j = 0; j < indexCount; j += 3)
			{
				std::vector<uint32_t> indices(3);
				stream.read(
					(char*)indices.data(),
					3 * sizeof(uint32_t)
				);

				result.push_back(vertices[indices[0]].Position);
				result.push_back(vertices[indices[1]].Position);
				result.push_back(vertices[indices[2]].Position);
			}
			break;
		}
	}

	GameDataResource::VPostLoad();

	return result;
}

auto Arg::Content::StaticModelResource::GetResourceFileExtension() const -> const std::string&
{
	return STATIC_MODEL_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::StaticModelResource::VPreLoad()
{
	GameDataResource::VPreLoad();
	m_pModelSpec = std::make_shared<Renderer::StaticModelSpec>();
	m_pModel = std::make_shared<Renderer::StaticModel>();
}

void Arg::Content::StaticModelResource::VOnLoad()
{
	GameDataResource::VOnLoad();
	const auto data = GetData()->data();
	Renderer::StaticModelData modelData{
		.Spec = *m_pModelSpec,
		.Meshes = std::vector<Renderer::StaticMeshData>(m_pModelSpec->MeshCount),
	};

	std::stringstream stream;
	stream.write((char*)data, GetData()->size() * sizeof(char));
	stream.read(
		(char*)modelData.Spec.MaterialIndices.data(),
		modelData.Spec.MaterialIndices.size() * sizeof(int32_t)
	);
	for (size_t i = 0; i < modelData.Spec.MeshCount; i++)
	{
		const size_t vertexCount = modelData.Spec.VertexCounts[i];
		const size_t indexCount = modelData.Spec.IndexCounts[i];
		modelData.Meshes[i].Vertices = std::vector<Renderer::StaticVertex>(vertexCount);
		modelData.Meshes[i].Indices = std::vector<uint32_t>(indexCount);
		stream.read(
			(char*)modelData.Meshes[i].Vertices.data(),
			vertexCount * sizeof(Renderer::StaticVertex)
		);
		stream.read(
			(char*)modelData.Meshes[i].Indices.data(),
			indexCount * sizeof(uint32_t)
		);
	}

	m_pModel->SetData(modelData);
}

auto Arg::Content::StaticModelResource::VGetSerializableData() const -> ISerializable*
{
	return m_pModelSpec.get();
}

void Arg::Content::StaticModelResource::VUnload()
{
	m_pModel = nullptr;
	m_pModelSpec = nullptr;
}
