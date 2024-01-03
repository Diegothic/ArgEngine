#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "StaticModel.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "RenderTarget.hpp"
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

			void DrawModel(
				const std::shared_ptr<StaticModel>& model,
				const Mat4& transform,
				const std::vector<Material*>& materials,
				bool bReceiveShadows,
				bool bCastShadows
			);

			void AddDirectionalLight(DirectionalLight& light);
			void AddPointLight(PointLight& light);
			void AddSpotLight(SpotLight& light);

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

			std::vector<Mat4> m_Transforms;
			std::unordered_map<GUID, Material*> m_Materials;
			std::unordered_map<GUID, std::vector<size_t>> m_MaterialStaticMeshIndices;
		};
	}
}
