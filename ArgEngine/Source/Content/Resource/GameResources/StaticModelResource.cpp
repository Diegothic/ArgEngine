#include <arg_pch.hpp>
#include "StaticModelResource.hpp"

Arg::Content::StaticModelResource::StaticModelResource(const std::shared_ptr<Resource>& resource)
	: GameDataResource(resource)
{

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
		.Spec = *(m_pModelSpec.get()),
		.Meshes = std::vector<Renderer::StaticMeshData>(modelData.Spec.MeshCount),
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

auto Arg::Content::StaticModelResource::VGetSerializableData() const->ISerializable*
{
	return m_pModelSpec.get();
}

void Arg::Content::StaticModelResource::VUnload()
{
	m_pModel = nullptr;
	m_pModelSpec = nullptr;
}
