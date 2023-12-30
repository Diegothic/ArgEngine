#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Renderer/ShaderProgram.hpp"
#include "Renderer/StaticMesh.hpp"
#include "Renderer/SkeletalMesh.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Camera/Camera.hpp"

namespace Arg
{
	namespace Renderer
	{
		class ShadowMap
		{
		public:
			ShadowMap(
				int32_t size,
				ShaderProgram* pShadowMapShader
			);
			~ShadowMap() = default;

			void Bind(int unit) const;

			void Begin(
				Renderer* renderer,
				const Mat4& lightSpaceTransform
			);
			void Draw(
				const StaticMesh& mesh,
				const Mat4& transform
			);
			void Draw(
				const SkeletalMesh& mesh,
				const Mat4& transform
			);
			void End();

		private:
			void InitializeBuffer(
				Texture& map,
				const FrameBuffer& buffer,
				int32_t size
			) const;

		private:
			Renderer* m_pRenderer = nullptr;
			ShaderProgram* m_pShader = nullptr;
			FrameBuffer m_Buffer;
			Texture m_Map;
			int32_t m_Size;
		};

		struct DirectionalLightSpec
		{
			Vec3 Direction = Vec3(0.0f, 0.0f, -1.0f);
			Vec3 Color = Vec3(1.0f);
			float Intensity = 1.0f;
			bool bCastShadows = true;
			ShaderProgram* pShadowMapShader = nullptr;
		};

		class DirectionalLight
		{
		public:
			DirectionalLight(const DirectionalLightSpec& spec);
			~DirectionalLight() = default;

			auto GetDirection() const -> const Vec3& { return m_Direction; }
			void SetDirection(const Vec3& direction) { m_Direction = Math::normalize(direction); }

			auto GetColor() const -> const Vec3& { return m_Color; }
			void SetColor(const Vec3& color) { m_Color = color; }

			auto GetIntensity() const -> const float& { return m_Intensity; }
			void SetIntensity(const float intensity) { m_Intensity = intensity; }

			auto IsCastingShadows() const -> const bool& { return m_bCastShadows; }
			void SetCastingShadows(bool bCastShadows);

			auto GetShadowMapShader() const -> ShaderProgram* { return m_pShadowMapShader; }
			void SetShadowMapShader(ShaderProgram* pShader);

			void Apply(
				ShaderProgram* shader,
				const Camera* camera
			) const;

			void BeginShadowMap(
				Renderer* renderer,
				const Camera* camera
			);
			void DrawToShadowMap(
				const StaticMesh& mesh,
				const Mat4& transform
			) const;
			void EndShadowMap();

			void BeginShadowMapFar(
				Renderer* renderer,
				const Camera* camera
			) const;
			void DrawToShadowMapFar(
				const StaticMesh& mesh,
				const Mat4& transform
			) const;
			void EndShadowMapFar();

		private:
			auto CalculateLightSpace(
				const Camera* camera
			) const -> Mat4;
			auto CalculateLightSpaceFar(
				const Camera* camera
			) const -> Mat4;
			void RefreshShadowMaps();

		private:
			Vec3 m_Direction;
			Vec3 m_Color;
			float m_Intensity;
			bool m_bCastShadows;
			ShaderProgram* m_pShadowMapShader = nullptr;
			std::unique_ptr<ShadowMap> m_pShadowMap = nullptr;
			std::unique_ptr<ShadowMap> m_pShadowMapFar = nullptr;
		};
	}
}
