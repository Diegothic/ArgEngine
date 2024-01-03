#include <arg_pch.hpp>
#include "RenderContext.hpp"

#include "ShaderProgram.hpp"
#include "Light/DirectionalLight.hpp"

Arg::Renderer::RenderContext::RenderContext(
	const RenderContextSpec& spec
)
	: m_Spec(spec)
{
}

void Arg::Renderer::RenderContext::DrawModel(
	const std::shared_ptr<StaticModel>& model,
	const Mat4& transform,
	const std::vector<Material*>& materials,
	bool bReceiveShadows,
	bool bCastShadows
)
{
	m_Transforms.push_back(transform);
	const size_t transformIndex = m_Transforms.size() - 1;
	for (size_t i = 0; i < model->GetMeshCount(); i++)
	{
		StaticMesh* mesh = model->GetMesh(i).get();
		m_StaticMeshes.push_back(mesh);
		const size_t staticMeshIndex = m_StaticMeshes.size() - 1;

		m_StaticMeshesDetails.emplace_back(transformIndex, bReceiveShadows, bCastShadows);

		const size_t materialIndex = model->GetMaterialIndex(i);
		Material* material = materials[materialIndex];
		const GUID materialID = material == nullptr
			                        ? GUID::Empty
			                        : material->GetID();

		if (!m_Materials.contains(materialID))
		{
			m_Materials[materialID] = material;
			m_MaterialStaticMeshIndices[materialID] = std::vector<size_t>();
			m_MaterialStaticMeshIndices[materialID].reserve(16);
		}

		m_MaterialStaticMeshIndices[materialID].push_back(staticMeshIndex);
	}
}

void Arg::Renderer::RenderContext::AddDirectionalLight(DirectionalLight& light)
{
	m_pDirectionalLight = &light;
}

void Arg::Renderer::RenderContext::AddPointLight(PointLight& light)
{
	m_PointLights.push_back(&light);
}

void Arg::Renderer::RenderContext::AddSpotLight(SpotLight& light)
{
	m_SpotLights.push_back(&light);
}

void Arg::Renderer::RenderContext::Render(
	Renderer& renderer,
	RenderTarget* renderTarget
) const
{
	if (m_Spec.pCamera == nullptr)
	{
		ARG_CONSOLE_LOG_WARN("No camera assigned!");
		return;
	}

	const Vec2i viewportSize = m_Spec.ViewportSize;
	const float aspectRatio = (float)viewportSize.x / (float)viewportSize.y;

	const Mat4 proj = m_Spec.pCamera->VGetProjection(aspectRatio);
	const Mat4 view = m_Spec.pCamera->GetView();

	auto shader = m_Spec.pBasicShader;

	if (shader == nullptr)
	{
		return;
	}

	renderTarget->End();

	if (m_pDirectionalLight != nullptr && m_pDirectionalLight->IsCastingShadows())
	{
		if (m_pDirectionalLight->GetShadowMapShader() == nullptr)
		{
			m_pDirectionalLight->SetShadowMapShader(m_Spec.pShadowMapShader);
		}

		{
			m_pDirectionalLight->BeginShadowMap(&renderer, m_Spec.pCamera);

			for (size_t i = 0; i < m_StaticMeshes.size(); i++)
			{
				const StaticMesh* mesh = m_StaticMeshes[i];
				const MeshDetails& meshDetails = m_StaticMeshesDetails[i];
				if (!meshDetails.bCastShadows)
				{
					continue;
				}

				const Mat4& transform = m_Transforms[meshDetails.TransformIndex];
				m_pDirectionalLight->DrawToShadowMap(*mesh, transform);
			}

			m_pDirectionalLight->EndShadowMap();
		}

		{
			m_pDirectionalLight->BeginShadowMapFar(&renderer, m_Spec.pCamera);

			for (size_t i = 0; i < m_StaticMeshes.size(); i++)
			{
				const StaticMesh* mesh = m_StaticMeshes[i];
				const MeshDetails& meshDetails = m_StaticMeshesDetails[i];
				if (!meshDetails.bCastShadows)
				{
					continue;
				}

				const Mat4& transform = m_Transforms[meshDetails.TransformIndex];
				m_pDirectionalLight->DrawToShadowMapFar(*mesh, transform);
			}

			m_pDirectionalLight->EndShadowMapFar();
		}
	}

	renderTarget->Begin();

	shader->Use();
	shader->SetUniform("u_Proj", proj);
	shader->SetUniform("u_View", view);

	shader->SetUniform("u_SkyBox", 0);

	shader->SetUniform("u_DirLightsCount", m_pDirectionalLight != nullptr ? 1 : 0);
	if (m_pDirectionalLight != nullptr)
	{
		m_pDirectionalLight->Apply(shader, m_Spec.pCamera);
	}
	shader->SetUniform("u_PointLightsCount", static_cast<int32_t>(m_PointLights.size()));
	shader->SetUniform("u_SpotLightsCount", static_cast<int32_t>(m_SpotLights.size()));

	for (int32_t i = 0; i < m_PointLights.size(); i++)
	{
		if (i > POINT_LIGHTS_MAX)
		{
			break;
		}

		m_PointLights[i]->Apply(shader, m_Spec.pCamera, i);
	}

	for (int32_t i = 0; i < m_SpotLights.size(); i++)
	{
		if (i > SPOT_LIGHTS_MAX)
		{
			break;
		}

		m_SpotLights[i]->Apply(shader, m_Spec.pCamera, i);
	}

	const FrameParams frameParams{
		.ViewportSize = viewportSize,
		.ClearColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f) // TODO: Camera->Background()
	};
	renderer.BeginFrame(frameParams);

	renderer.BeginOpaque();

	for (const GUID& materialID : m_Materials | std::ranges::views::keys)
	{
		const Material* material = m_Materials.at(materialID);
		const std::vector<size_t>& meshIndices = m_MaterialStaticMeshIndices.at(materialID);
		for (const size_t& meshIndex : meshIndices)
		{
			const StaticMesh* mesh = m_StaticMeshes[meshIndex];
			const MeshDetails& meshDetails = m_StaticMeshesDetails[meshIndex];

			const Mat4& transform = m_Transforms[meshDetails.TransformIndex];
			const Mat3 normal(Math::transpose(Math::inverse(view * transform)));

			shader->SetUniform("u_Model", transform);
			shader->SetUniform("u_Normal", normal);

			shader->SetUniform("u_ReceiveShadows", meshDetails.bReceiveShadows);

			if (materialID != GUID::Empty)
			{
				material->Apply(shader);
			}
			else
			{
				shader->SetUniform("u_Material.diffuse", Vec3(0.5f));
				shader->SetUniform("u_Material.diffuseMap", 0);

				shader->SetUniform("u_Material.specular", 0.4f);
				shader->SetUniform("u_Material.specularMap", 0);
				shader->SetUniform("u_Material.shininess", 0.2f);

				shader->SetUniform("u_Material.reflection", 0.0f);
				shader->SetUniform("u_Material.reflectionMap", 0);
			}

			mesh->Draw();
		}
	}

	renderer.EndFrame();
}
