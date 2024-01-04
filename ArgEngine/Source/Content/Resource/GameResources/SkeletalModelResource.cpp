#include <arg_pch.hpp>
#include "SkeletalModelResource.hpp"

Arg::Content::SkeletalModelResource::SkeletalModelResource(const std::shared_ptr<Resource>& resource)
	: GameDataResource(resource)
{
}

auto Arg::Content::SkeletalModelResource::GetResourceFileExtension() const -> const std::string&
{
	return SKELETAL_MODEL_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::SkeletalModelResource::VPreLoad()
{
	GameDataResource::VPreLoad();
	m_pModelSpec = std::make_unique<Renderer::SkeletalModelSpec>();
	m_pModel = std::make_unique<Renderer::SkeletalModel>();
}

void Arg::Content::SkeletalModelResource::VOnLoad()
{
	GameDataResource::VOnLoad();
	const auto data = GetData()->data();
	Renderer::SkeletalModelData modelData{
		.Spec = *m_pModelSpec,
		.Meshes = std::vector<Renderer::SkeletalMeshData>(m_pModelSpec->MeshCount),
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
		modelData.Meshes[i].Vertices = std::vector<Renderer::SkeletalVertex>(vertexCount);
		modelData.Meshes[i].Indices = std::vector<uint32_t>(indexCount);
		stream.read(
			(char*)modelData.Meshes[i].Vertices.data(),
			vertexCount * sizeof(Renderer::SkeletalVertex)
		);
		stream.read(
			(char*)modelData.Meshes[i].Indices.data(),
			indexCount * sizeof(uint32_t)
		);
	}

	m_pModel->SetData(modelData);
}

auto Arg::Content::SkeletalModelResource::VGetSerializableData() const -> ISerializable*
{
	return m_pModelSpec.get();
}

void Arg::Content::SkeletalModelResource::VUnload()
{
	m_pModel = nullptr;
	m_pModelSpec = nullptr;
}
