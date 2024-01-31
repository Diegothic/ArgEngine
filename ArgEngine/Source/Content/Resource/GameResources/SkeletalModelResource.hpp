#pragma once

#include <arg_pch.hpp>

#include "Debug/Assert.hpp"
#include "Content/Resource/Resource.hpp"
#include "Content/Resource/GameResources/GameDataResource.hpp"
#include "Renderer/SkeletalModel.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string SKELETAL_MODEL_RESOURCE_FILE_EXTENSION = ".askmodel";

		class SkeletalModelResource : public GameDataResource
		{
		public:
			SkeletalModelResource(const std::shared_ptr<Resource>& resource);
			SkeletalModelResource(const SkeletalModelResource&) = delete;
			virtual ~SkeletalModelResource() = default;

			auto GetModel() const -> const Renderer::SkeletalModel&
			{
				ARG_ASSERT(m_pModel != nullptr);
				return *m_pModel;
			}

		protected:
			auto GetResourceFileExtension() const -> const std::string& override;

			void VPreLoad() override;
			void VOnLoad() override;
			auto VGetSerializableData() const->ISerializable* override;
			void VUnload() override;

		private:
			std::unique_ptr<Renderer::SkeletalModelSpec> m_pModelSpec = nullptr;
			std::unique_ptr<Renderer::SkeletalModel> m_pModel = nullptr;
		};
	}
}
