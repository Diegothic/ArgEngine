#pragma once

#include <arg_pch.hpp>

#include "ShaderProgram.hpp"
#include "StaticMesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"

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
			void Initialize();

			void BeginFrame(const FrameParams& params) const;
			void EndFrame() const;

			void BeginOpaque() const;
			void BeginTransparent() const;

			void BeginSkybox() const;
			void EndSkybox() const;

			void BeginDebug(float lineWidth = 1.0f) const;
			void EndDebug() const;

		private:
			std::unique_ptr<Texture> m_pWhiteTexture = nullptr;
			std::unique_ptr<Texture> m_pBlackTexture = nullptr;
		};
	}
}
