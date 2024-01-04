#pragma once

#include <arg_pch.hpp>

#include "SkeletalMesh.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct SkeletalModelSpec : public Content::YamlSerializable
		{
			size_t MeshCount;
			std::vector<int32_t> MaterialIndices;
			std::vector<size_t> VertexCounts;
			std::vector<size_t> IndexCounts;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		struct SkeletalModelData
		{
			SkeletalModelSpec Spec;
			std::vector<SkeletalMeshData> Meshes;
		};

		class SkeletalModel
		{
		public:
			SkeletalModel() = default;
			SkeletalModel(const SkeletalModel&) = delete;
			~SkeletalModel() = default;

			auto GetMeshCount() const -> size_t { return m_Meshes.size(); }
			auto GetMesh(size_t meshIndex) const -> const SkeletalMesh&;

			auto GetMaterialCount() const -> size_t { return m_Spec.MaterialIndices.size(); }
			auto GetMaterialIndex(size_t meshIndex) const -> int32_t;
			auto GetVertexCount(size_t meshIndex) const -> size_t;
			auto GetIndexCount(size_t meshIndex) const -> size_t;

			void SetData(const SkeletalModelData& data);

		private:
			SkeletalModelSpec m_Spec;
			std::vector<std::unique_ptr<SkeletalMesh>> m_Meshes;
		};
	}
}
