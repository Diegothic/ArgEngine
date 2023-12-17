#include <arg_pch.hpp>
#include "RenderContext.hpp"

#include "Debug/Assert.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
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

		m_StaticMeshTransformIndices.push_back(transformIndex);

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

void Arg::Renderer::RenderContext::Render(
	const Renderer& renderer
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

	const FrameParams frameParams{
		.ViewportSize = viewportSize,
		.ClearColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f) // TODO: Camera->Background()
	};
	renderer.BeginFrame(frameParams);
	renderer.EndFrame();

	renderer.BeginOpaque();

	auto shader = m_Spec.pBasicShader;

	if (shader == nullptr)
	{
		return;
	}

	shader->Use();
	shader->SetUniform("u_Proj", proj);
	shader->SetUniform("u_View", view);

	// TEMP: Sunlight
	const DirectionalLightSpec dirLightSpec
	{
		.Direction = Vec3(-0.3f, -0.4f, -1.0f),
		.Color = Vec3(1.0f),
		.Intensity = 1.0f,
		.bCastShadows = false,
		.ShadowMapShader = nullptr
	};
	DirectionalLight dirLight(dirLightSpec);

	shader->SetUniform("u_SkyBox", 0);

	shader->SetUniform("u_PointLightsCount", 0);
	shader->SetUniform("u_SpotLightsCount", 0);
	shader->SetUniform("u_DirLightsCount", 1);

	dirLight.Apply(shader, m_Spec.pCamera);

	// TODO: Per mesh
	shader->SetUniform("u_ReceiveShadows", false);

	for (const GUID& materialID : m_Materials | std::ranges::views::keys)
	{
		const Material* material = m_Materials.at(materialID);
		const std::vector<size_t>& meshIndices = m_MaterialStaticMeshIndices.at(materialID);
		for (const size_t& meshIndex : meshIndices)
		{
			const StaticMesh* mesh = m_StaticMeshes[meshIndex];
			const Mat4& transform = m_Transforms[m_StaticMeshTransformIndices[meshIndex]];
			const Mat3 normal(Math::transpose(Math::inverse(view * transform)));

			shader->SetUniform("u_Model", transform);
			shader->SetUniform("u_Normal", normal);

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

	// for (size_t i = 0; i < m_StaticMeshes.size(); i++)
	// {
	// 	const auto& staticMesh = m_StaticMeshes[i];
	// 	const auto& transform = m_Transforms[m_StaticMeshTransformIndices[i]];
	//
	// 	const Mat4& model = transform;
	// 	const Mat3 normal(Math::transpose(Math::inverse(view * model)));
	//
	// 	shader->SetUniform("u_Model", model);
	// 	shader->SetUniform("u_Normal", normal);
	//
	// 	shader->SetUniform("u_ReceiveShadows", false);
	//
	//
	// 	if (m_StaticMeshMaterials[i] != nullptr)
	// 	{
	// 		m_StaticMeshMaterials[i]->Apply(shader);
	// 	}
	// 	else
	// 	{
	// 		shader->SetUniform("u_Material.diffuse", Vec3(0.5f));
	// 		shader->SetUniform("u_Material.diffuseMap", 0);
	//
	// 		shader->SetUniform("u_Material.specular", 0.4f);
	// 		shader->SetUniform("u_Material.specularMap", 0);
	// 		shader->SetUniform("u_Material.shininess", 0.2f);
	//
	// 		shader->SetUniform("u_Material.reflection", 0.0f);
	// 		shader->SetUniform("u_Material.reflectionMap", 0);
	// 	}
	//
	// 	staticMesh->Draw();
	// }
}
