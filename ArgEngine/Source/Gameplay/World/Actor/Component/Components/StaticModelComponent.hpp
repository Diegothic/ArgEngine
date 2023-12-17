#pragma once

#include <arg_pch.hpp>
#include "Gameplay/World/Actor/Component/ActorComponent.hpp"
#include "Content/ResourceCache.hpp"
#include "Content/Resource/GameResources/StaticModelResource.hpp"
#include "Content/Resource/GameResources/MaterialResource.hpp"

namespace Arg
{
	using MaterialHandle = Content::ResourceHandle<Content::MaterialResource>;
	using StaticModelHandle = Content::ResourceHandle<Content::StaticModelResource>;

	namespace Gameplay
	{
		class StaticModelComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			StaticModelComponent();

			auto VCreateDefault() const->std::shared_ptr<ActorComponent> override;

			auto VGetID() const->GUID override { return COMPONENT_ID; };
			auto VGetName() const-> const std::string& override { return COMPONENT_NAME; };

			void VRender(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

			auto GetStaticModel() const ->Content::ResourceHandle<Content::StaticModelResource> { return m_StaticModel; }
			void SetStaticModel(const Content::ResourceHandle<Content::StaticModelResource>& staticModel);

			auto GetMaterialCount() const -> size_t {return m_Materials.size();}
			auto GetMaterial(size_t index) const -> const MaterialHandle&;
			void SetMaterial(size_t index, const MaterialHandle& material);

			auto GetReceiveShadows() const -> bool { return m_bReceiveShadows; }
			void SetReceiveShadows(bool bReceiveShadows);

			auto GetCastShadows() const -> bool { return m_bCastShadows; }
			void SetCastShadows(bool bCastShadows);

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			StaticModelHandle m_StaticModel;
			std::vector<MaterialHandle> m_Materials;

			bool m_bReceiveShadows = true;
			bool m_bCastShadows = true;
		};
	}
}
