#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/GameResource.hpp"
#include "Renderer/Material.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string MATERIAL_RESOURCE_FILE_EXTENSION = ".amat";

		class MaterialResource : public GameResource
		{
		public:
			MaterialResource(const std::shared_ptr<Resource>& resource);
			MaterialResource(const MaterialResource&) = delete;
			~MaterialResource() = default;

			auto GetMaterial() const -> const std::shared_ptr<Renderer::Material>&
			{
				ARG_ASSERT(m_pMaterial != nullptr);
				return m_pMaterial;
			}

		protected:
			auto GetResourceFileExtension() const -> const std::string& override;

			void VPreLoad() override;
			auto VGetSerializableData() const -> ISerializable* override;
			void VPostLoad() override;
			void VUnload() override;

		private:
			std::shared_ptr<Renderer::Material> m_pMaterial = nullptr;
		};
	}
}
