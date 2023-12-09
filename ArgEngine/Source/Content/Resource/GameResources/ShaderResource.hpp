#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/GameResource.hpp"
#include "Renderer/ShaderProgram.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string SHADER_RESOURCE_FILE_EXTENSION = ".ashader";

		struct ShaderResourceSpec : public YamlSerializable
		{
			std::string VertexSourceFile;
			std::string FragmentSourceFile;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		class ShaderResource : public GameResource
		{
		public:
			ShaderResource(const std::shared_ptr<Resource>& resource);
			ShaderResource(const ShaderResource&) = delete;
			~ShaderResource() = default;

			auto GetShader() const -> const std::shared_ptr<Renderer::ShaderProgram>&
			{
				return m_pShader;
			}

		public:
			auto VIsSaveable() const -> bool override { return false; }

		protected:
			auto GetResourceFileExtension() const -> const std::string & override;

			void VPreLoad() override;
			void VOnLoad() override;
			auto VGetSerializableData() const->ISerializable* override;
			void VUnload() override;

		private:
			std::unique_ptr<ShaderResourceSpec> m_pSpec = nullptr;
			std::shared_ptr<Renderer::ShaderProgram> m_pShader = nullptr;
		};
	}
}
