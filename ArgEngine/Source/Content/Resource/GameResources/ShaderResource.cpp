#include <arg_pch.hpp>
#include "ShaderResource.hpp"

#include "Debug/Assert.hpp"

auto Arg::Content::ShaderResourceSpec::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["Shader"];
	header["Vertex"] = VertexSourceFile;
	header["Fragment"] = FragmentSourceFile;

	return true;
}

auto Arg::Content::ShaderResourceSpec::VOnDeserialize(const YAML::Node& node) -> bool
{
	auto& header = node["Shader"];
	if (!header)
	{
		return false;
	}

	VertexSourceFile = ValueOr<std::string>(header["Vertex"], "");
	FragmentSourceFile = ValueOr<std::string>(header["Fragment"], "");

	return true;
}

Arg::Content::ShaderResource::ShaderResource(const std::shared_ptr<Resource>& resource)
	: GameResource(resource)
{

}

auto Arg::Content::ShaderResource::GetResourceFileExtension() const -> const std::string&
{
	return SHADER_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::ShaderResource::VPreLoad()
{
	m_pSpec = std::make_unique<ShaderResourceSpec>();
	m_pShader = std::make_shared<Renderer::ShaderProgram>();
}

void Arg::Content::ShaderResource::VOnLoad()
{
	const auto shaderSourcePath = GetResource()->GetPath() / "Source";
	ARG_ASSERT_M(std::filesystem::exists(shaderSourcePath), "Shader source path does not exist!");
	const auto vertexShaderFilePath = shaderSourcePath / m_pSpec->VertexSourceFile;
	ARG_ASSERT_M(std::filesystem::exists(vertexShaderFilePath), "Shader source file does not exist!");
	const auto fragmentShaderFilePath = shaderSourcePath / m_pSpec->FragmentSourceFile;
	ARG_ASSERT_M(std::filesystem::exists(fragmentShaderFilePath), "Shader source file does not exist!");

	std::stringstream buffer;
	std::ifstream vertexSourceFile(vertexShaderFilePath);
	buffer.str(std::string());
	buffer << vertexSourceFile.rdbuf();
	const std::string vertSource = buffer.str();
	vertexSourceFile.close();

	std::ifstream fragmentSourceFile(fragmentShaderFilePath);
	buffer.str(std::string());
	buffer << fragmentSourceFile.rdbuf();
	const std::string fragSource = buffer.str();
	fragmentSourceFile.close();

	const Renderer::ShaderSource shaderSource{
			.VertexSource = vertSource,
			.FragmentSource = fragSource,
	};

	m_pShader->Create(shaderSource);
}

auto Arg::Content::ShaderResource::VGetSerializableData() const->ISerializable*
{
	return m_pSpec.get();
}

void Arg::Content::ShaderResource::VUnload()
{
	m_pShader = nullptr;
	m_pSpec = nullptr;
}
