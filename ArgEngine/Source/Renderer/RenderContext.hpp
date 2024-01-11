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
		struct RenderContextSpec
		{
			Camera* pCamera = nullptr;
			Vec2i ViewportSize = Vec2i(1920, 1080);
			ShaderProgram* pBasicShader = nullptr;
			ShaderProgram* pShadowMapShader = nullptr;
			ShaderProgram* pSkyboxShader = nullptr;
			StaticModel* pSkyboxMesh = nullptr;
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

			void Render(
				Renderer& renderer,
				RenderTarget* renderTarget
			) const;

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

			Vec3 m_BackgroundColor = Vec3(0.5f);
			CubeMap* m_pCubeMap = nullptr;
		};
	}
}
