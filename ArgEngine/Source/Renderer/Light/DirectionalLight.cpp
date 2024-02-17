#include <arg_pch.hpp>
#include "DirectionalLight.hpp"

#include "Debug/Assert.hpp"
#include "Gameplay/World/Actor/Actor.hpp"

Arg::Renderer::ShadowMap::ShadowMap(
	int32_t size,
	ShaderProgram* pShadowMapShader
)
	: m_pShader(pShadowMapShader),
	  m_Size(size)
{
	m_Map.Bind();
	{
		const TextureData mapData{
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
	Renderer* renderer,
	const Mat4& lightSpaceTransform)
{
	m_pRenderer = renderer;
	m_Buffer.Bind();
	const FrameParams frameParams{
		.ViewportSize = Vec2i(m_Size),
		.ClearColor = Vec4(1.0f)
	};
	renderer->BeginFrame(frameParams);

	m_pShader->Use();
	m_pShader->SetUniform("u_LightSpace", lightSpaceTransform);
}

void Arg::Renderer::ShadowMap::Draw(
	const StaticMesh& mesh,
	const Mat4& transform
) const
{
	m_pShader->SetUniform("u_IsSkeletal", false);
	m_pShader->SetUniform("u_Model", transform);
	mesh.Draw();
}

void Arg::Renderer::ShadowMap::Draw(
	const SkeletalMesh& mesh,
	const Mat4& transform,
	const SkeletonPose& pose
) const
{
	m_pShader->SetUniform("u_IsSkeletal", true);
	m_pShader->SetUniform("u_Model", transform);
	pose.Apply(m_pShader);
	mesh.Draw();
}

void Arg::Renderer::ShadowMap::End()
{
	m_Buffer.Unbind();
}

void Arg::Renderer::ShadowMap::InitializeBuffer(
	Texture& map,
	const FrameBuffer& buffer,
	int32_t size
) const
{
	map.Bind();
	{
		const TextureData mapData{
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

Arg::Renderer::DirectionalLight::DirectionalLight(
	const DirectionalLightSpec& spec
)
	: m_Direction(spec.Direction),
	  m_Color(spec.Color),
	  m_Intensity(spec.Intensity),
	  m_bCastShadows(spec.bCastShadows),
	  m_pShadowMapShader(spec.pShadowMapShader)
{
	RefreshShadowMaps();
}

void Arg::Renderer::DirectionalLight::SetCastingShadows(bool bCastShadows)
{
	if (m_bCastShadows == bCastShadows)
	{
		return;
	}

	m_bCastShadows = bCastShadows;
	RefreshShadowMaps();
}

void Arg::Renderer::DirectionalLight::SetShadowMapShader(ShaderProgram* pShader)
{
	m_pShadowMapShader = pShader;
	RefreshShadowMaps();
}

void Arg::Renderer::DirectionalLight::Apply(
	ShaderProgram* shader,
	const Camera* camera
) const
{
	shader->SetUniform("u_DirLightSpace.lightSpace", CalculateLightSpace(camera));
	shader->SetUniform("u_DirLightSpace.lightSpaceFar", CalculateLightSpaceFar(camera));
	shader->SetUniform("u_DirLightsCount", 1);
	shader->SetUniform("u_DirLights[0].properties.ambient", m_Color * m_Intensity * 0.2f);
	shader->SetUniform("u_DirLights[0].properties.diffuse", m_Color * m_Intensity);
	shader->SetUniform("u_DirLights[0].properties.specular", m_Color * m_Intensity);
	const Vec3 directionNormalized = Math::normalize(m_Direction);
	const Vec3 lightDirectionView = Vec3(camera->GetView()
		* Vec4(directionNormalized, 0.0f));
	shader->SetUniform("u_DirLights[0].direction", lightDirectionView);

	shader->SetUniform("u_DirLights[0].castShadows", m_bCastShadows);
	if (m_bCastShadows)
	{
		m_pShadowMap->Bind(10);
		shader->SetUniform("u_DirLights[0].shadowMap", 10);
		m_pShadowMapFar->Bind(11);
		shader->SetUniform("u_DirLights[0].shadowMapFar", 11);
	}
}

void Arg::Renderer::DirectionalLight::BeginShadowMap(
	Renderer* renderer,
	const Camera* camera
)
{
	ARG_ASSERT_M(m_pShadowMap != nullptr, "Shadow map missing!");
	m_pShadowMap->Begin(
		renderer,
		CalculateLightSpace(camera)
	);
}

void Arg::Renderer::DirectionalLight::DrawToShadowMap(
	const StaticMesh& mesh,
	const Mat4& transform
) const
{
	ARG_ASSERT_M(m_pShadowMap != nullptr, "Shadow map missing!");
	m_pShadowMap->Draw(mesh, transform);
}

void Arg::Renderer::DirectionalLight::DrawToShadowMap(
	const SkeletalMesh& mesh,
	const Mat4& transform,
	const SkeletonPose& pose
) const
{
	ARG_ASSERT_M(m_pShadowMap != nullptr, "Shadow map missing!");
	m_pShadowMap->Draw(mesh, transform, pose);
}

void Arg::Renderer::DirectionalLight::EndShadowMap()
{
	ARG_ASSERT_M(m_pShadowMap != nullptr, "Shadow map missing!");
	m_pShadowMap->End();
}

void Arg::Renderer::DirectionalLight::BeginShadowMapFar(
	Renderer* renderer,
	const Camera* camera
) const
{
	ARG_ASSERT_M(m_pShadowMapFar != nullptr, "Shadow map missing!");
	m_pShadowMapFar->Begin(
		renderer,
		CalculateLightSpaceFar(camera)
	);
}

void Arg::Renderer::DirectionalLight::DrawToShadowMapFar(
	const StaticMesh& mesh,
	const Mat4& transform) const
{
	ARG_ASSERT_M(m_pShadowMapFar != nullptr, "Shadow map missing!");
	m_pShadowMapFar->Draw(mesh, transform);
}

void Arg::Renderer::DirectionalLight::EndShadowMapFar()
{
	ARG_ASSERT_M(m_pShadowMapFar != nullptr, "Shadow map missing!");
	m_pShadowMapFar->End();
}

auto Arg::Renderer::DirectionalLight::CalculateLightSpace(
	const Camera* camera
) const -> Mat4
{
	const Mat4 lightProj = Math::ortho(
		-25.0f, 25.0f,
		-25.0f, 25.0f,
		0.1f, 70.0f
	);
	const Vec3 directionNormalized = Math::normalize(m_Direction);
	const Vec3 arbitrary = m_Direction.z != 0.0f ? Vec3(1.0f, 0.0f, 0.0f) : Vec3(0.0f, 0.0f, 1.0f);
	const Vec3 perp = Math::normalize(Math::cross(directionNormalized, arbitrary));
	const Vec3 up = Math::normalize(Math::cross(perp, directionNormalized));
	const Mat4 lightView = Math::lookAt(
		-directionNormalized,
		Vec3(0.0f),
		up
	);
	Mat4 lightModel(1.0f);
	lightModel = glm::translate(
		lightModel,
		-camera->GetPosition()
	);
	lightModel = glm::translate(
		lightModel,
		directionNormalized * 35.0f
	);
	return lightProj * lightView * lightModel;
}

auto Arg::Renderer::DirectionalLight::CalculateLightSpaceFar(
	const Camera* camera
) const -> Mat4
{
	const Mat4 lightProj = Math::ortho(
		-100.0f, 100.0f,
		-100.0f, 100.0f,
		0.1f, 1000.0f
	);
	const Vec3 directionNormalized = Math::normalize(m_Direction);
	const Vec3 arbitrary = m_Direction.z != 0.0f ? Vec3(1.0f, 0.0f, 0.0f) : Vec3(0.0f, 0.0f, 1.0f);
	const Vec3 perp = Math::normalize(Math::cross(directionNormalized, arbitrary));
	const Vec3 up = Math::normalize(Math::cross(perp, directionNormalized));
	const Mat4 lightView = Math::lookAt(
		-directionNormalized,
		Vec3(0.0f),
		up
	);
	Mat4 lightModel(1.0f);
	lightModel = glm::translate(
		lightModel,
		-camera->GetPosition()
	);
	lightModel = glm::translate(
		lightModel,
		directionNormalized * 500.0f
	);
	return lightProj * lightView * lightModel;
}

void Arg::Renderer::DirectionalLight::RefreshShadowMaps()
{
	if (m_bCastShadows)
	{
		m_pShadowMap = std::make_unique<ShadowMap>(8192, m_pShadowMapShader);
		m_pShadowMapFar = std::make_unique<ShadowMap>(4096, m_pShadowMapShader);
	}
	else
	{
		m_pShadowMap = nullptr;
		m_pShadowMapFar = nullptr;
	}
}
