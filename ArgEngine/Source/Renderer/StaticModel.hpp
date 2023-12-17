#pragma once

#include <arg_pch.hpp>

#include "StaticMesh.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct StaticModelSpec : public Content::YamlSerializable
		{
			size_t MeshCount;
			std::vector<int32_t> MaterialIndices;
			std::vector<size_t> VertexCounts;
			std::vector<size_t> IndexCounts;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		struct StaticModelData
		{
			StaticModelSpec Spec;
			std::vector<StaticMeshData> Meshes;
		};

		class StaticModel
		{
		public:
			StaticModel() = default;
			StaticModel(const StaticModel&) = delete;
			~StaticModel() = default;

			auto GetMeshCount() const -> size_t { return m_Meshes.size(); }
			auto GetMesh(size_t meshIndex) const -> const std::shared_ptr<StaticMesh>&;

			auto GetMaterialCount() const -> size_t { return m_Spec.MaterialIndices.size(); }
			auto GetMaterialIndex(size_t meshIndex) const -> int32_t;
			auto GetVertexCount(size_t meshIndex) const -> size_t;
			auto GetIndexCount(size_t meshIndex) const -> size_t;

			void SetData(const StaticModelData& data);

		private:
			StaticModelSpec m_Spec;
			std::vector<std::shared_ptr<StaticMesh>> m_Meshes;
		};
	}
}
