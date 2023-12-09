#include <arg_pch.hpp>
#include "Material.hpp"

Arg::Renderer::Material::Material(Content::ResourceCache* pResourceCache)
	: m_pResourceCache(pResourceCache)
{

}

void Arg::Renderer::Material::Apply(const std::shared_ptr<ShaderProgram>& shader)
{
	if (m_DiffuseMap.IsValid())
	{
		m_DiffuseMap.Get()->GetTexture()->Bind(TEXTURE_UNIT_DIFFUSE);
		shader->SetUniform("u_Material.diffuseMap", TEXTURE_UNIT_DIFFUSE);
		shader->SetUniform("u_Material.diffuse", Vec3(1.0f));
	}
	else
	{
		shader->SetUniform("u_Material.diffuseMap", 0);
		shader->SetUniform("u_Material.diffuse", Vec3(m_DiffuseColor));
	}

	if (m_SpecularMap.IsValid())
	{
		m_SpecularMap.Get()->GetTexture()->Bind(TEXTURE_UNIT_SPECULAR);
		shader->SetUniform("u_Material.specularMap", TEXTURE_UNIT_SPECULAR);
		shader->SetUniform("u_Material.specular", 1.0f);
	}
	else
	{
		shader->SetUniform("u_Material.specularMap", 0);
		shader->SetUniform("u_Material.specular", m_Specular);
	}

	shader->SetUniform("u_Material.shininess", m_Shininess);


	if (m_ReflectivityMap.IsValid())
	{
		m_ReflectivityMap.Get()->GetTexture()->Bind(TEXTURE_UNIT_REFLECTIVITY);
		shader->SetUniform("u_Material.reflectionMap", TEXTURE_UNIT_REFLECTIVITY);
		shader->SetUniform("u_Material.reflection", 1.0f);
	}
	else
	{
		shader->SetUniform("u_Material.reflectionMap", 0);
		shader->SetUniform("u_Material.reflection", m_Reflectivity);
	}
}

void Arg::Renderer::Material::SetDiffuseMap(const TextureHandle diffuseMap)
{
	if (m_DiffuseMap.IsValid())
	{
		m_DiffuseMap.Get()->FreeRef();
	}

	m_DiffuseMap = diffuseMap;

	if (m_DiffuseMap.IsValid())
	{
		m_DiffuseMap.Get()->AddRef();
	}
}

void Arg::Renderer::Material::SetSpecularMap(const TextureHandle specularMap)
{
	if (m_SpecularMap.IsValid())
	{
		m_SpecularMap.Get()->FreeRef();
	}

	m_SpecularMap = specularMap;

	if (m_SpecularMap.IsValid())
	{
		m_SpecularMap.Get()->AddRef();
	}
}

void Arg::Renderer::Material::SetReflectivityMap(const TextureHandle reflectivityMap)
{
	if (m_ReflectivityMap.IsValid())
	{
		m_ReflectivityMap.Get()->FreeRef();
	}

	m_ReflectivityMap = reflectivityMap;

	if (m_ReflectivityMap.IsValid())
	{
		m_ReflectivityMap.Get()->AddRef();
	}
}

void Arg::Renderer::Material::Reserve() const
{
	if (m_DiffuseMap.IsValid())
	{
		m_DiffuseMap.Get()->AddRef();
	}

	if (m_SpecularMap.IsValid())
	{
		m_SpecularMap.Get()->AddRef();
	}

	if (m_ReflectivityMap.IsValid())
	{
		m_ReflectivityMap.Get()->AddRef();
	}
}

void Arg::Renderer::Material::Release() const
{
	if (m_DiffuseMap.IsValid())
	{
		m_DiffuseMap.Get()->FreeRef();
	}

	if (m_SpecularMap.IsValid())
	{
		m_SpecularMap.Get()->FreeRef();
	}

	if (m_ReflectivityMap.IsValid())
	{
		m_ReflectivityMap.Get()->FreeRef();
	}
}

auto Arg::Renderer::Material::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["Material"];

	header["DiffuseMapID"] = m_DiffuseMap.GetID();
	header["DiffuseColor"] = m_DiffuseColor;

	header["SpecularMapID"] = m_SpecularMap.GetID();
	header["Specular"] = m_Specular;
	header["Shininess"] = m_Shininess;

	header["ReflectivityMapID"] = m_ReflectivityMap.GetID();
	header["Reflectivity"] = m_Reflectivity;

	return true;
}

auto Arg::Renderer::Material::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto& header = node["Material"];
	if (!header)
	{
		return false;
	}

	const GUID diffuseMapID = ValueOr<GUID>(header["DiffuseMapID"], GUID::Empty);
	m_DiffuseMap = m_pResourceCache->CreateHandle<Content::TextureResource>(diffuseMapID);
	m_DiffuseColor = ValueOr<Vec4>(header["DiffuseColor"], Vec4(1.0f));

	const GUID specularMapID = ValueOr<GUID>(header["SpecularMapID"], GUID::Empty);
	m_SpecularMap = m_pResourceCache->CreateHandle<Content::TextureResource>(specularMapID);
	m_Specular = ValueOr<float>(header["Specular"], 0.5f);
	m_Shininess = ValueOr<float>(header["Shininess"], 0.1f);

	const GUID reflectivityMapID = ValueOr<GUID>(header["ReflectivityMapID"], GUID::Empty);
	m_ReflectivityMap = m_pResourceCache->CreateHandle<Content::TextureResource>(reflectivityMapID);
	m_Reflectivity = ValueOr<float>(header["Reflectivity"], 0.0f);

	return true;
}
