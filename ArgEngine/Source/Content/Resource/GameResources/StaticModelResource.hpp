#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/Resource.hpp"
#include "Content/Resource/GameResources/GameDataResource.hpp"
#include "Renderer/StaticModel.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string STATIC_MODEL_RESOURCE_FILE_EXTENSION = ".amodel";

		class StaticModelResource : public GameDataResource
		{
		public:
			StaticModelResource(const std::shared_ptr<Resource>& resource);
			StaticModelResource(const StaticModelResource&) = delete;
			virtual ~StaticModelResource() = default;

			auto GetStaticModel() const -> const std::shared_ptr<Renderer::StaticModel>&
			{
				return m_pModel;
			}

			auto FindMeshData(size_t meshIndex) -> std::vector<Vec3>;

		protected:
			auto GetResourceFileExtension() const -> const std::string& override;

			void VPreLoad() override;
			void VOnLoad() override;
			auto VGetSerializableData() const -> ISerializable* override;
			void VUnload() override;

		private:
			std::shared_ptr<Renderer::StaticModelSpec> m_pModelSpec = nullptr;
			std::shared_ptr<Renderer::StaticModel> m_pModel = nullptr;
		};
	}
}
