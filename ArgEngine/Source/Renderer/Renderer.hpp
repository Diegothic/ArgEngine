#pragma once

#include <arg_pch.hpp>

#include "StaticMesh.hpp"
#include "Material.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct FrameParams
		{
			Vec2i ViewportSize = Vec2i(1920.0f, 1080.0f);
			Vec4 ClearColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		};

		class Renderer
		{
		public:

			void BeginFrame(const FrameParams& params) const;

			void BeginOpaque() const;
			void BeginTransparent() const;

			void DrawStaticMesh(
				const std::shared_ptr<const StaticMesh>& mesh,
				const std::shared_ptr<const Material>& material,
				const Mat4& transform
			) const;
		};
	}
}
