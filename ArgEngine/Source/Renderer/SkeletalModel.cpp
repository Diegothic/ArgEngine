#include <arg_pch.hpp>
#include "SkeletalModel.hpp"

#include "Debug/Assert.hpp"

auto Arg::Renderer::SkeletalModel::GetMesh(size_t meshIndex) const -> const SkeletalMesh&
{
	ARG_ASSERT(meshIndex < m_Meshes.size());
	return *m_Meshes[meshIndex];
}

auto Arg::Renderer::SkeletalModel::GetMaterialIndex(size_t meshIndex) const -> int32_t
{
	ARG_ASSERT(meshIndex < m_Meshes.size());
	return m_Spec.MaterialIndices[meshIndex];
}

auto Arg::Renderer::SkeletalModel::GetVertexCount(size_t meshIndex) const -> size_t
{
	ARG_ASSERT(meshIndex < m_Meshes.size());
	return m_Spec.VertexCounts[meshIndex];
}

auto Arg::Renderer::SkeletalModel::GetIndexCount(size_t meshIndex) const -> size_t
{
	ARG_ASSERT(meshIndex < m_Meshes.size());
	return m_Spec.IndexCounts[meshIndex];
}

void Arg::Renderer::SkeletalModel::SetData(const SkeletalModelData& data)
{
	ARG_ASSERT(data.Meshes.size() == data.Spec.MaterialIndices.size());
	m_Meshes.clear();

	m_Spec = data.Spec;
	m_Meshes = std::vector<std::unique_ptr<SkeletalMesh>>(m_Spec.MeshCount);
	for (size_t i = 0; i < m_Spec.MeshCount; i++)
	{
		m_Meshes[i] = std::make_unique<SkeletalMesh>();
		m_Meshes[i]->SetData(data.Meshes[i]);
	}
}

auto Arg::Renderer::SkeletalModelSpec::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["SkeletalModel"];
	header["MeshCount"] = MeshCount;
	for (size_t i = 0; i < MeshCount; i++)
	{
		auto meshNode = header["Meshes"][i];
		meshNode["MaterialIndex"] = MaterialIndices[i];
		meshNode["VertexCount"] = VertexCounts[i];
		meshNode["IndexCount"] = IndexCounts[i];
	}

	node["SkeletalModel"] = header;

	return true;
}

auto Arg::Renderer::SkeletalModelSpec::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto header = node["SkeletalModel"];
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
