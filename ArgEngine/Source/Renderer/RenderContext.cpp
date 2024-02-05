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

void Arg::Renderer::RenderContext::SetCamera(Camera* pCamera)
{
	m_Spec.pCamera = pCamera;
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

void Arg::Renderer::RenderContext::DrawSkeletalModel(
	const SkeletonPose& pose,
	const SkeletalModel& model,
	const Mat4& transform,
	const std::vector<Material*>& materials,
	bool bReceiveShadows,
	bool bCastShadows
)
{
	m_Transforms.push_back(transform);
	const size_t transformIndex = m_Transforms.size() - 1;
	for (size_t i = 0; i < model.GetMeshCount(); i++)
	{
		const SkeletalMesh* mesh = &model.GetMesh(i);
		m_SkeletalMeshes.push_back(mesh);
		const size_t meshIndex = m_SkeletalMeshes.size() - 1;

		m_SkeletalMeshesDetails.emplace_back(transformIndex, bReceiveShadows, bCastShadows);
		m_SkeletalMeshesPoses.push_back(&pose);

		const size_t materialIndex = model.GetMaterialIndex(i);
		Material* material = materials[materialIndex];
		const GUID materialID = material == nullptr
			                        ? GUID::Empty
			                        : material->GetID();

		if (!m_Materials.contains(materialID))
		{
			m_Materials[materialID] = material;
			m_MaterialSkeletalMeshIndices[materialID] = std::vector<size_t>();
			m_MaterialSkeletalMeshIndices[materialID].reserve(16);
		}

		m_MaterialSkeletalMeshIndices[materialID].push_back(meshIndex);
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

void Arg::Renderer::RenderContext::SetBackgroundColor(const Vec3& color)
{
	m_BackgroundColor = color;
}

void Arg::Renderer::RenderContext::SetSkybox(CubeMap* cubeMap)
{
	m_pCubeMap = cubeMap;
}

void Arg::Renderer::RenderContext::DrawDebugLine(
	const Vec3& from,
	const Vec3& to,
	const Vec3& color
)
{
	const Vec3 lineVec = to - from;
	const float lineLength = Math::length(lineVec);
	const Vec3 lineVecNormalized = Math::normalize(lineVec);
	const float rotZ = Math::atan(
		lineVecNormalized.y,
		lineVecNormalized.x
	);
	const Vec3 forwardVec = Vec3(lineVecNormalized.x, lineVecNormalized.y, 0.0f);
	const float dotForward = Math::length(forwardVec) > 0.0f
		                         ? Math::dot(
			                         lineVecNormalized,
			                         Math::normalize(forwardVec)
		                         )
		                         : 0.0f;
	const float dotUp = Math::dot(lineVecNormalized, Vec3(0.0f, 0.0f, 1.0f));
	const float rotY = -Math::atan(
		dotUp,
		dotForward
	);

	const Mat4 transform = Math::CalculateTransform(
		from,
		Quat(Vec3(0.0f, rotY, rotZ)),
		Vec3(lineLength, 1.0f, 1.0f)
	);
	m_DebugLines.push_back({transform, color});
}

void Arg::Renderer::RenderContext::DrawDebugBox(
	const Vec3& position,
	const Vec3& rotation,
	const Vec3& size,
	const Vec3& color
)
{
	const Mat4 transform = Math::CalculateTransform(position, rotation, size);
	m_DebugBoxes.push_back({transform, color});
}

void Arg::Renderer::RenderContext::DrawDebugSphere(
	const Vec3& position,
	const Vec3& rotation,
	const float& radius,
	const Vec3& color
)
{
	const Mat4 transform = Math::CalculateTransform(position, rotation, Vec3(radius));
	m_DebugSpheres.push_back({transform, color});
}

void Arg::Renderer::RenderContext::DrawDebugCylinder(
	const Vec3& position,
	const Vec3& rotation,
	const float& radius,
	const float& height,
	const Vec3& color
)
{
	const Mat4 transform = Math::CalculateTransform(position, rotation, Vec3(radius, radius, height));
	m_DebugCylinders.push_back({transform, color});
}

void Arg::Renderer::RenderContext::DrawDebugCapsule(
	const Vec3& position,
	const Vec3& rotation,
	const float& radius,
	const float& height,
	const Vec3& color
)
{
	const float cylinderHeight = Math::max(height, 0.0f);
	DrawDebugCylinder(position, rotation, radius, cylinderHeight, color);
	const Vec3 upVec = Math::UpVecFromRotation(rotation);
	const Vec3 offsetVec = (upVec * cylinderHeight * 0.5f);
	DrawDebugSphere(position + offsetVec, rotation, radius, color);
	DrawDebugSphere(position - offsetVec, rotation, radius, color);
}

void Arg::Renderer::RenderContext::DrawDebugCamera(
	const Vec3& position,
	const Vec3& rotation,
	const Vec3& color
)
{
	const Mat4 transform = Math::CalculateTransform(position, rotation, Vec3(0.5f));
	m_DebugCameras.push_back({transform, color});
}

void Arg::Renderer::RenderContext::DrawDebugCustomMesh(
	const std::shared_ptr<StaticModel>& model,
	const Vec3& position,
	const Vec3& rotation,
	const Vec3& scale,
	const Vec3& color
)
{
	const Mat4 transform = Math::CalculateTransform(position, rotation, scale);
	StaticMesh* pMesh = model->GetMesh(0).get();
	m_DebugCustomMeshes.push_back({
		pMesh,
		{transform, color}
	});
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

	m_Spec.pCamera->SetAspectRatio(aspectRatio);
	const Mat4 proj = m_Spec.pCamera->VGetProjection(aspectRatio);
	const Mat4 view = m_Spec.pCamera->GetView();

	auto shader = m_Spec.pBasicShader;

	if (shader == nullptr)
	{
		return;
	}

	if (renderTarget != nullptr)
	{
		renderTarget->End();
	}

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

			for (size_t i = 0; i < m_SkeletalMeshes.size(); i++)
			{
				const SkeletalMesh* mesh = m_SkeletalMeshes[i];
				const MeshDetails& meshDetails = m_SkeletalMeshesDetails[i];
				if (!meshDetails.bCastShadows)
				{
					continue;
				}

				const SkeletonPose* pose = m_SkeletalMeshesPoses[i];
				const Mat4& transform = m_Transforms[meshDetails.TransformIndex];
				m_pDirectionalLight->DrawToShadowMap(*mesh, transform, *pose);
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

			for (size_t i = 0; i < m_SkeletalMeshes.size(); i++)
			{
				const SkeletalMesh* mesh = m_SkeletalMeshes[i];
				const MeshDetails& meshDetails = m_SkeletalMeshesDetails[i];
				if (!meshDetails.bCastShadows)
				{
					continue;
				}

				const SkeletonPose* pose = m_SkeletalMeshesPoses[i];
				const Mat4& transform = m_Transforms[meshDetails.TransformIndex];
				m_pDirectionalLight->DrawToShadowMap(*mesh, transform, *pose);
			}

			m_pDirectionalLight->EndShadowMapFar();
		}
	}

	if (renderTarget != nullptr)
	{
		renderTarget->Begin();
	}

	shader->Use();
	shader->SetUniform("u_Proj", proj);
	shader->SetUniform("u_View", view);

	shader->SetUniform("u_SkyboxColor", m_BackgroundColor);
	if (m_pCubeMap != nullptr)
	{
		shader->SetUniform("u_UsingSkybox", true);
		m_pCubeMap->Bind(TEXTURE_UNIT_SKYBOX);
		shader->SetUniform("u_Skybox", TEXTURE_UNIT_SKYBOX);
	}
	else
	{
		shader->SetUniform("u_UsingSkybox", false);
		shader->SetUniform("u_Skybox", TEXTURE_UNIT_WHITE);
	}

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
		.ClearColor = Vec4(m_BackgroundColor, 1.0f)
	};
	renderer.BeginFrame(frameParams);

	renderer.BeginOpaque();

	for (const GUID& materialID : m_Materials | std::ranges::views::keys)
	{
		const Material* material = m_Materials.at(materialID);
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

		shader->SetUniform("u_IsSkeletal", false);

		if (m_MaterialStaticMeshIndices.contains(materialID))
		{
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

				mesh->Draw();
			}
		}

		shader->SetUniform("u_IsSkeletal", true);

		if (m_MaterialSkeletalMeshIndices.contains(materialID))
		{
			const std::vector<size_t>& skeletalMeshIndices = m_MaterialSkeletalMeshIndices.at(materialID);
			for (const size_t& meshIndex : skeletalMeshIndices)
			{
				const SkeletalMesh* mesh = m_SkeletalMeshes[meshIndex];
				const MeshDetails& meshDetails = m_SkeletalMeshesDetails[meshIndex];

				const Mat4& transform = m_Transforms[meshDetails.TransformIndex];
				const Mat3 normal(Math::transpose(Math::inverse(view * transform)));

				shader->SetUniform("u_Model", transform);
				shader->SetUniform("u_Normal", normal);

				const SkeletonPose* pose = m_SkeletalMeshesPoses[meshIndex];
				pose->Apply(shader);

				shader->SetUniform("u_ReceiveShadows", meshDetails.bReceiveShadows);

				mesh->Draw();
			}
		}
	}

	if (m_pCubeMap != nullptr)
	{
		renderer.BeginSkybox();

		auto skyboxShader = m_Spec.pSkyboxShader;
		auto skyboxMesh = m_Spec.pSkyboxMesh;

		skyboxShader->Use();

		skyboxShader->SetUniform("u_Proj", proj);

		const Mat4 skyBoxView = Mat4(Mat3(view));
		skyboxShader->SetUniform("u_View", skyBoxView);

		m_pCubeMap->Bind(TEXTURE_UNIT_SKYBOX);
		skyboxShader->SetUniform("u_SkyBox", TEXTURE_UNIT_SKYBOX);

		skyboxMesh->GetMesh(0)->Draw();

		m_pCubeMap->Unbind();

		renderer.EndSkybox();
	}

	{
		renderer.BeginDebug(2.0f);

		auto debugShader = m_Spec.pDebugShader;

		debugShader->Use();

		debugShader->SetUniform("u_Proj", proj);
		debugShader->SetUniform("u_View", view);

		const StaticMesh* pLineMesh = m_Spec.pLineMesh->GetMesh(0).get();
		for (const auto& transformColor : m_DebugLines)
		{
			debugShader->SetUniform("u_Model", transformColor.Transform);
			debugShader->SetUniform("u_Color", transformColor.Color);

			pLineMesh->Draw();
		}

		const StaticMesh* pBoxMesh = m_Spec.pBoxMesh->GetMesh(0).get();
		for (const auto& transformColor : m_DebugBoxes)
		{
			debugShader->SetUniform("u_Model", transformColor.Transform);
			debugShader->SetUniform("u_Color", transformColor.Color);

			pBoxMesh->Draw();
		}

		const StaticMesh* pSphereMesh = m_Spec.pSphereMesh->GetMesh(0).get();
		for (const auto& transformColor : m_DebugSpheres)
		{
			debugShader->SetUniform("u_Model", transformColor.Transform);
			debugShader->SetUniform("u_Color", transformColor.Color);

			pSphereMesh->Draw();
		}

		const StaticMesh* pCylinderMesh = m_Spec.pCylinderMesh->GetMesh(0).get();
		for (const auto& transformColor : m_DebugCylinders)
		{
			debugShader->SetUniform("u_Model", transformColor.Transform);
			debugShader->SetUniform("u_Color", transformColor.Color);

			pCylinderMesh->Draw();
		}

		const StaticMesh* pCameraMesh = m_Spec.pCameraMesh->GetMesh(0).get();
		for (const auto& transformColor : m_DebugCameras)
		{
			debugShader->SetUniform("u_Model", transformColor.Transform);
			debugShader->SetUniform("u_Color", transformColor.Color);

			pCameraMesh->Draw();
		}

		for (const auto& customMesh : m_DebugCustomMeshes)
		{
			const StaticMesh* pMesh = customMesh.pMesh;
			const TransformColor& transformColor = customMesh.Spec;

			debugShader->SetUniform("u_Model", transformColor.Transform);
			debugShader->SetUniform("u_Color", transformColor.Color);

			pMesh->Draw();
		}

		renderer.EndDebug();
	}

	renderer.EndFrame();
}
