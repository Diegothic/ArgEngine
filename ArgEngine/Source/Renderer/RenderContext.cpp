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
	const std::shared_ptr<Material>& material,
	bool bReceiveShadows,
	bool bCastShadows
)
{
	m_Transforms.push_back(transform);
	for (size_t i = 0; i < model->GetMeshCount(); i++)
	{
		m_StaticMeshes.push_back(model->GetMesh(i));
		m_StaticMeshTransformIndices.push_back(m_Transforms.size() - 1);
		m_StaticMeshMaterials.push_back(material);
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
	const float aspectRatio = (float)viewportSize.x / viewportSize.y;

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

	// TODO: Make shared ptrs
	shader->SetUniform("u_DirLightsCount", 1);

	dirLight.Apply(shader, m_Spec.pCamera);

	/*shader->SetUniform("u_DirLights[0].properties.ambient", dirLightSpec.Color * dirLightSpec.Intensity * 0.2f);
	shader->SetUniform("u_DirLights[0].properties.diffuse", dirLightSpec.Color * dirLightSpec.Intensity);
	shader->SetUniform("u_DirLights[0].properties.specular", dirLightSpec.Color * dirLightSpec.Intensity);
	const Vec3 lightDirectionView = Vec3(m_Spec.pCamera->GetView()
		* Vec4(dirLightSpec.Direction, 0.0f));
	shader->SetUniform("u_DirLights[0].direction", lightDirectionView);
	shader->SetUniform("u_DirLights[0].castShadows", false);
	shader->SetUniform("u_DirLights[0].shadowMap", 0);
	shader->SetUniform("u_DirLights[0].shadowMapFar", 0);*/

	for (size_t i = 0; i < m_StaticMeshes.size(); i++)
	{
		const auto& staticMesh = m_StaticMeshes[i];
		const auto& transform = m_Transforms[m_StaticMeshTransformIndices[i]];

		const Mat4& model = transform;
		const Mat3 normal(Math::transpose(Math::inverse(view * model)));

		shader->SetUniform("u_Model", model);
		shader->SetUniform("u_Normal", normal);

		shader->SetUniform("u_ReceiveShadows", false);

		

		if (m_StaticMeshMaterials[i] != nullptr)
		{
			m_StaticMeshMaterials[i]->Apply(shader);
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

		staticMesh->Draw();
	}
}
