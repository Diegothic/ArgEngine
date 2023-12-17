#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "StaticModel.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "Camera/Camera.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct RenderContextSpec
		{
			std::shared_ptr<Camera> pCamera = nullptr;
			Vec2i ViewportSize = Vec2i(1920, 1080);
			std::shared_ptr<ShaderProgram> pBasicShader = nullptr;
		};

		class RenderContext
		{
		public:
			RenderContext(const RenderContextSpec& spec);

			void DrawModel(
				const std::shared_ptr<StaticModel>& model,
				const Mat4& transform,
				const std::vector<Material*>& materials,
				bool bReceiveShadows,
				bool bCastShadows
			);

			void Render(
				const Renderer& renderer
			) const;

		private:
			RenderContextSpec m_Spec;
			
			std::vector<StaticMesh*> m_StaticMeshes;
			std::vector<Mat4> m_Transforms;
			std::vector<size_t> m_StaticMeshTransformIndices;
			std::unordered_map<GUID, Material*> m_Materials;
			std::unordered_map<GUID, std::vector<size_t>> m_MaterialStaticMeshIndices;
			
			//std::vector<std::shared_ptr<Material>> m_StaticMeshMaterials;

		};
	}
}
