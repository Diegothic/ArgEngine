#pragma once

#include <arg_pch.hpp>

#include "CubeMap.hpp"
#include "Core/Math/Math.hpp"
#include "StaticModel.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "RenderTarget.hpp"
#include "SkeletalModel.hpp"
#include "Animation/Skeleton.hpp"
#include "Camera/Camera.hpp"
#include "Light/DirectionalLight.hpp"
#include "Light/PointLight.hpp"
#include "Light/SpotLight.hpp"

namespace Arg
{
	namespace Renderer
	{
		constexpr Vec3 DEBUG_COLOR_PHYSICS = Vec3(0.0f, 1.0f, 0.0f);
		constexpr Vec3 DEBUG_COLOR_PHYSICS_TRIGGER = Vec3(0.0f, 0.5f, 0.12f);
		constexpr Vec3 DEBUG_COLOR_LIGHT = Vec3(1.0f, 0.8f, 0.3f);
		constexpr Vec3 DEBUG_COLOR_SPECIAL = Vec3(0.4f, 0.0f, 1.0f);
		constexpr Vec3 DEBUG_COLOR_USER = Vec3(1.0f, 1.0f, 1.0f);

		struct RenderContextSpec
		{
			Camera* pCamera = nullptr;
			Vec2i ViewportSize = Vec2i(1920, 1080);
			bool bDebugEnabled = false;

			ShaderProgram* pBasicShader = nullptr;
			ShaderProgram* pShadowMapShader = nullptr;
			ShaderProgram* pSkyboxShader = nullptr;
			ShaderProgram* pDebugShader = nullptr;

			StaticModel* pSkyboxMesh = nullptr;
			StaticModel* pLineMesh = nullptr;
			StaticModel* pBoxMesh = nullptr;
			StaticModel* pSphereMesh = nullptr;
			StaticModel* pCylinderMesh = nullptr;
			StaticModel* pCameraMesh = nullptr;
		};

		class RenderContext
		{
		public:
			struct MeshDetails
			{
				size_t TransformIndex;
				bool bReceiveShadows;
				bool bCastShadows;
			};

		public:
			RenderContext(const RenderContextSpec& spec);

			void SetCamera(Camera* pCamera);

			void DrawModel(
				const std::shared_ptr<StaticModel>& model,
				const Mat4& transform,
				const std::vector<Material*>& materials,
				bool bReceiveShadows,
				bool bCastShadows
			);

			void DrawSkeletalModel(
				const SkeletonPose& pose,
				const SkeletalModel& model,
				const Mat4& transform,
				const std::vector<Material*>& materials,
				bool bReceiveShadows,
				bool bCastShadows
			);

			void AddDirectionalLight(DirectionalLight& light);
			void AddPointLight(PointLight& light);
			void AddSpotLight(SpotLight& light);

			void SetBackgroundColor(const Vec3& color);
			void SetSkybox(CubeMap* cubeMap);

			void DrawDebugLine(
				const Vec3& from,
				const Vec3& to,
				const Vec3& color
			);

			void DrawDebugBox(
				const Vec3& position,
				const Vec3& rotation,
				const Vec3& size,
				const Vec3& color
			);

			void DrawDebugSphere(
				const Vec3& position,
				const Vec3& rotation,
				const float& radius,
				const Vec3& color
			);

			void DrawDebugCylinder(
				const Vec3& position,
				const Vec3& rotation,
				const float& radius,
				const float& height,
				const Vec3& color
			);

			void DrawDebugCapsule(
				const Vec3& position,
				const Vec3& rotation,
				const float& radius,
				const float& height,
				const Vec3& color
			);

			void DrawDebugCamera(
				const Vec3& position,
				const Vec3& rotation,
				const Vec3& color
			);

			void DrawDebugCustomMesh(
				const std::shared_ptr<StaticModel>& model,
				const Vec3& position,
				const Vec3& rotation,
				const Vec3& scale,
				const Vec3& color
			);

			void Render(
				Renderer& renderer,
				RenderTarget* renderTarget
			) const;

		private:
			struct TransformColor
			{
				Mat4 Transform;
				Vec3 Color;
			};

			struct CustomMesh
			{
				StaticMesh* pMesh;
				TransformColor Spec;
			};

		private:
			RenderContextSpec m_Spec;

			DirectionalLight* m_pDirectionalLight = nullptr;
			std::vector<PointLight*> m_PointLights;
			std::vector<SpotLight*> m_SpotLights;

			std::vector<StaticMesh*> m_StaticMeshes;
			std::vector<MeshDetails> m_StaticMeshesDetails;

			std::vector<const SkeletalMesh*> m_SkeletalMeshes;
			std::vector<MeshDetails> m_SkeletalMeshesDetails;
			std::vector<const SkeletonPose*> m_SkeletalMeshesPoses;

			std::vector<Mat4> m_Transforms;
			std::unordered_map<GUID, Material*> m_Materials;
			std::unordered_map<GUID, std::vector<size_t>> m_MaterialStaticMeshIndices;
			std::unordered_map<GUID, std::vector<size_t>> m_MaterialSkeletalMeshIndices;

			std::vector<TransformColor> m_DebugLines;
			std::vector<TransformColor> m_DebugBoxes;
			std::vector<TransformColor> m_DebugSpheres;
			std::vector<TransformColor> m_DebugCylinders;
			std::vector<TransformColor> m_DebugCameras;
			std::vector<CustomMesh> m_DebugCustomMeshes;

			Vec3 m_BackgroundColor = Vec3(0.5f);
			CubeMap* m_pCubeMap = nullptr;
		};
	}
}
