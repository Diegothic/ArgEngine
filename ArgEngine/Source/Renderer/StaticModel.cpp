#include <arg_pch.hpp>
#include "StaticModel.hpp"

#include "Debug/Assert.hpp"

auto Arg::Renderer::StaticModel::GetMesh(size_t meshIndex) const->const std::shared_ptr<StaticMesh>&
{
	ARG_ASSERT(meshIndex >= 0 && meshIndex < m_Meshes.size(), "Index out of range!");
	return m_Meshes[meshIndex];
}

auto Arg::Renderer::StaticModel::GetMaterialIndex(size_t meshIndex) const->int32_t
{
	ARG_ASSERT(meshIndex >= 0 && meshIndex < m_Meshes.size(), "Index out of range!");
	return m_Spec.MaterialIndices[meshIndex];
}

auto Arg::Renderer::StaticModel::GetVertexCount(size_t meshIndex) const->size_t
{
	ARG_ASSERT(meshIndex >= 0 && meshIndex < m_Meshes.size(), "Index out of range!");
	return m_Spec.VertexCounts[meshIndex];
}

auto Arg::Renderer::StaticModel::GetIndexCount(size_t meshIndex) const->size_t
{
	ARG_ASSERT(meshIndex >= 0 && meshIndex < m_Meshes.size(), "Index out of range!");
	return m_Spec.IndexCounts[meshIndex];
}

void Arg::Renderer::StaticModel::SetData(const StaticModelData& data)
{
	ARG_ASSERT(data.Meshes.size() == data.Spec.MaterialIndices.size(), "Invalid model data!");
	m_Meshes.clear();

	m_Spec = data.Spec;
	m_Meshes = std::vector<std::shared_ptr<StaticMesh>>(m_Spec.MeshCount);
	for (size_t i = 0; i < m_Spec.MeshCount; i++)
	{
		m_Meshes[i] = std::make_shared<StaticMesh>();
		m_Meshes[i]->SetData(data.Meshes[i]);
	}
}

auto Arg::Renderer::StaticModelSpec::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["StaticModel"];
	header["MeshCount"] = MeshCount;
	for (size_t i = 0; i < MeshCount; i++)
	{
		auto meshNode = header["Meshes"][i];
		meshNode["MaterialIndex"] = MaterialIndices[i];
		meshNode["VertexCount"] = VertexCounts[i];
		meshNode["IndexCount"] = IndexCounts[i];
	}

	node["StaticModel"] = header;

	return true;
}

auto Arg::Renderer::StaticModelSpec::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto header = node["StaticModel"];
	if (!header)
	{
		return false;
	}

	MeshCount = ValueOr<size_t>(header["MeshCount"], 0);
	if (header["Meshes"])
	{
		MaterialIndices = std::vector<int32_t>(MeshCount);
		VertexCounts = std::vector<size_t>(MeshCount);
		IndexCounts = std::vector<size_t>(MeshCount);
		for (size_t i = 0; i < MeshCount; i++)
		{
			auto meshNode = header["Meshes"][i];
			MaterialIndices[i] = ValueOr<int32_t>(meshNode["MaterialIndex"], 0);
			VertexCounts[i] = ValueOr<size_t>(meshNode["VertexCount"], 0);
			IndexCounts[i] = ValueOr<size_t>(meshNode["IndexCount"], 0);
		}
	}

	return true;
}
