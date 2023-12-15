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
				const std::shared_ptr<Material>& material,
				bool bReceiveShadows,
				bool bCastShadows
			);

			void Render(
				const Renderer& renderer
			) const;

		private:
			RenderContextSpec m_Spec;
			
			std::vector<Mat4> m_Transforms;

			std::vector<std::shared_ptr<StaticMesh>> m_StaticMeshes;
			std::vector<std::shared_ptr<Material>> m_StaticMeshMaterials;
			std::vector<size_t> m_StaticMeshTransformIndices;
		};
	}
}
