#include <arg_pch.hpp>
#include "DirectionalLight.hpp"

#include "Debug/Assert.hpp"

Arg::Renderer::ShadowMap::ShadowMap(
	const int32_t size,
	const std::shared_ptr<ShaderProgram>& shadowMapShader
)
	: m_Size(size),
	m_Shader(shadowMapShader)
{
	m_Map.Bind();
	{
		TextureData mapData{
			.Data = nullptr,
			.Width = m_Size,
			.Height = m_Size,
			.Format = TextureFormat::FormatDepth
		};
		m_Map.SetData(mapData);
		m_Map.SetFilter(TextureFilter::FilterNearest);
		m_Map.SetWrapMode(TextureWrapMode::WrapModeClampToBorder);
		m_Map.SetBorderColor(Vec4(1.0f));
	}

	m_Buffer.Bind();
	m_Map.AttachToFrameBuffer(TextureAttachmentSlot::AttachmentSlotDepth);
	m_Buffer.DisableColorBuffers();
	m_Buffer.Unbind();
	m_Map.Unbind();
}

void Arg::Renderer::ShadowMap::Bind(int unit) const
{
	m_Map.Bind(unit);
}

void Arg::Renderer::ShadowMap::Begin(
	const std::shared_ptr<Renderer>& renderer,
	const Mat4& lightSpaceTransform)
{
	m_Renderer = renderer;
	m_Buffer.Bind();
	const FrameParams frameParams{
				.ViewportSize = Vec2i(m_Size),
				.ClearColor = Vec4(1.0f)
	};
	renderer->BeginFrame(frameParams);

	m_Shader->Use();
	m_Shader->SetUniform("u_LightSpace", lightSpaceTransform);
}

void Arg::Renderer::ShadowMap::Draw(
	const std::shared_ptr<const StaticMesh>& mesh,
	const Mat4& transform
)
{
	m_Shader->SetUniform("u_IsSkeletal", false);
	m_Shader->SetUniform("u_Model", transform);
	mesh->Draw();
}

void Arg::Renderer::ShadowMap::End()
{
	m_Buffer.Unbind();
}

void Arg::Renderer::ShadowMap::InitializeBuffer(
	const Texture& map,
	const FrameBuffer& buffer,
	const int32_t size
) const
{
	map.Bind();
	{
		TextureData mapData{
			.Data = nullptr,
			.Width = size,
			.Height = size,
			.Format = TextureFormat::FormatDepth
		};
		map.SetData(mapData);
		map.SetFilter(TextureFilter::FilterNearest);
		map.SetWrapMode(TextureWrapMode::WrapModeClampToBorder);
		map.SetBorderColor(Vec4(1.0f));
	}

	buffer.Bind();
	map.AttachToFrameBuffer(TextureAttachmentSlot::AttachmentSlotDepth);
	buffer.DisableColorBuffers();
	buffer.Unbind();
	map.Unbind();
}

Arg::Renderer::DirectionalLight::DirectionalLight(const DirectionalLightSpec& spec)
	: m_Direction(Math::normalize(spec.Direction)),
	m_Color(spec.Color),
	m_Intensity(spec.Intensity),
	m_bCastShadows(spec.bCastShadows),
	m_ShadowMapShader(spec.ShadowMapShader)
{
	RefreshShadowMaps();
}

void Arg::Renderer::DirectionalLight::SetCastingShadows(const bool bCastShadows)
{
	if (m_bCastShadows == bCastShadows)
	{
		return;
	}

	m_bCastShadows = bCastShadows;
	RefreshShadowMaps();
}

void Arg::Renderer::DirectionalLight::Apply(
	const std::shared_ptr<ShaderProgram>& shader,
	const std::shared_ptr<const Camera>& camera
) const
{
	shader->SetUniform("u_DirLightSpace.lightSpace", CalculateLightSpace(camera));
	shader->SetUniform("u_DirLightSpace.lightSpaceFar", CalculateLightSpaceFar(camera));
	shader->SetUniform("u_DirLightsCount", 1);
	shader->SetUniform("u_DirLights[0].properties.ambient", m_Color * m_Intensity * 0.2f);
	shader->SetUniform("u_DirLights[0].properties.diffuse", m_Color * m_Intensity);
	shader->SetUniform("u_DirLights[0].properties.specular", m_Color * m_Intensity);
	const Vec3 lightDirectionView = Vec3(camera->GetView()
		* Vec4(m_Direction, 0.0f));
	shader->SetUniform("u_DirLights[0].direction", lightDirectionView);

	shader->SetUniform("u_DirLights[0].castShadows", m_bCastShadows);
	if (m_bCastShadows)
	{
		m_ShadowMap->Bind(10);
		shader->SetUniform("u_DirLights[0].shadowMap", 10);
		m_ShadowMapFar->Bind(11);
		shader->SetUniform("u_DirLights[0].shadowMapFar", 11);
	}
}

void Arg::Renderer::DirectionalLight::BeginShadowMap(
	const std::shared_ptr<Renderer>& renderer,
	const std::shared_ptr<Camera>& camera
)
{
	ARG_ASSERT(m_ShadowMap != nullptr, "Shadow map missing!");
	m_ShadowMap->Begin(
		renderer,
		CalculateLightSpace(camera)
	);
}

void Arg::Renderer::DirectionalLight::DrawToShadowMap(
	const std::shared_ptr<const StaticMesh>& mesh,
	const Mat4& transform
) const
{
	ARG_ASSERT(m_ShadowMap != nullptr, "Shadow map missing!");
	m_ShadowMap->Draw(mesh, transform);
}

void Arg::Renderer::DirectionalLight::EndShadowMap()
{
	ARG_ASSERT(m_ShadowMap != nullptr, "Shadow map missing!");
	m_ShadowMap->End();
}

void Arg::Renderer::DirectionalLight::BeginShadowMapFar(
	const std::shared_ptr<Renderer>& renderer,
	const std::shared_ptr<Camera>& camera
) const
{
	ARG_ASSERT(m_ShadowMapFar != nullptr, "Shadow map missing!");
	m_ShadowMapFar->Begin(
		renderer,
		CalculateLightSpaceFar(camera)
	);
}

void Arg::Renderer::DirectionalLight::DrawToShadowMapFar(
	const std::shared_ptr<const StaticMesh>& mesh,
	const Mat4& transform) const
{
	ARG_ASSERT(m_ShadowMapFar != nullptr, "Shadow map missing!");
	m_ShadowMapFar->Draw(mesh, transform);
}

void Arg::Renderer::DirectionalLight::EndShadowMapFar()
{
	ARG_ASSERT(m_ShadowMapFar != nullptr, "Shadow map missing!");
	m_ShadowMapFar->End();
}

auto Arg::Renderer::DirectionalLight::CalculateLightSpace(
	const std::shared_ptr<const Camera>& camera
) const -> Mat4
{
	const Mat4 lightProj = Math::ortho(
		-35.0f, 35.0f,
		-35.0f, 35.0f,
		0.1f, 75.0f
	);
	const Mat4 lightView = Math::lookAt(
		-m_Direction * 20.0f
		+ camera->GetPosition()
		+ camera->GetForwardVector() * 5.0f,
		Vec3(0.0f) + camera->GetPosition()
		+ camera->GetForwardVector() * 5.0f,
		Vec3(0.0f, 0.0f, 1.0f)
	);
	return lightProj * lightView;
}

auto Arg::Renderer::DirectionalLight::CalculateLightSpaceFar(
	const std::shared_ptr<const Camera>& camera
) const -> Mat4
{
	Mat4 lightProj = Math::ortho(
		-100.0f, 100.0f,
		-100.0f, 100.0f,
		0.1f, 500.0f
	);
	Mat4 lightView = Math::lookAt(
		-m_Direction * 200.0f
		+ camera->GetPosition()
		+ camera->GetForwardVector() * 5.0f,
		Vec3(0.0f) + camera->GetPosition()
		+ camera->GetForwardVector() * 5.0f,
		Vec3(0.0f, 0.0f, 1.0f)
	);
	return lightProj * lightView;
}

void Arg::Renderer::DirectionalLight::RefreshShadowMaps()
{
	if (m_bCastShadows)
	{
		m_ShadowMap = std::unique_ptr<ShadowMap>(new ShadowMap(4096, m_ShadowMapShader));
		m_ShadowMapFar = std::unique_ptr<ShadowMap>(new ShadowMap(2048, m_ShadowMapShader));
	}
	else
	{
		m_ShadowMap = nullptr;
		m_ShadowMapFar = nullptr;
	}
}
