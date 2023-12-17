#pragma once

#include <arg_pch.hpp>

#include "ShaderProgram.hpp"
#include "Content/Resource/GameResources/TextureResource.hpp"
#include "Content/ResourceCache.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	using TextureHandle = Content::ResourceHandle<Content::TextureResource>;

	namespace Renderer
	{
		constexpr int32_t TEXTURE_UNIT_DIFFUSE = 2;
		constexpr int32_t TEXTURE_UNIT_SPECULAR = 3;
		constexpr int32_t TEXTURE_UNIT_REFLECTIVITY = 4;

		class Material : public Content::YamlSerializable
		{
		public:
			Material(Content::Resource* pResource);
			Material(const Material&) = delete;

		public:
			auto GetID() const -> GUID { return m_pResource->GetID(); }

		public:
			void Apply(const std::shared_ptr<ShaderProgram>& shader) const;

		public:
			auto GetDiffuseMap() const -> const TextureHandle& { return m_DiffuseMap; }
			void SetDiffuseMap(const TextureHandle& diffuseMap);

			auto GetDiffuseColor() const -> const Vec4& { return m_DiffuseColor; }
			void SetDiffuseColor(const Vec4& color) { m_DiffuseColor = color; }

			auto GetSpecularMap() const -> const TextureHandle& { return m_SpecularMap; }
			void SetSpecularMap(const TextureHandle& specularMap);

			auto GetSpecular() const -> const float& { return m_Specular; }
			void SetSpecular(const float& specular) { m_Specular = specular; }

			auto GetShininess() const -> const float& { return m_Shininess; }
			void SetShininess(const float& shininess) { m_Shininess = shininess; }

			auto GetReflectivityMap() const -> const TextureHandle& { return m_ReflectivityMap; }
			void SetReflectivityMap(const TextureHandle& reflectivityMap);

			auto GetReflectivity() const -> const float& { return m_Reflectivity; }
			void SetReflectivity(const float& reflectivity) { m_Reflectivity = reflectivity; }

		public:
			void Reserve() const;
			void Release() const;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			Content::Resource* m_pResource = nullptr;
			//Content::ResourceCache* m_pResourceCache = nullptr;

			TextureHandle m_DiffuseMap;
			Vec4 m_DiffuseColor = Vec4(1.0f);

			TextureHandle m_SpecularMap;
			float m_Specular = 0.5f;
			float m_Shininess = 0.1f;

			TextureHandle m_ReflectivityMap;
			float m_Reflectivity = 0.0f;
		};
	}
}
