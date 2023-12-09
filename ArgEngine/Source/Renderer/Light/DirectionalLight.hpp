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
				const int32_t size,
				const std::shared_ptr<ShaderProgram>& shadowMapShader
			);
			~ShadowMap() = default;

			void Bind(int unit) const;

			void Begin(
				const std::shared_ptr<Renderer>& renderer,
				const Mat4& lightSpaceTransform
			);
			void Draw(
				const std::shared_ptr<const StaticMesh>& mesh,
				const Mat4& transform
			);
			void Draw(
				const std::shared_ptr<const SkeletalMesh>& mesh,
				const Mat4& transform
			);
			void End();

		private:
			void InitializeBuffer(
				Texture& map,
				const FrameBuffer& buffer,
				const int32_t size
			) const;

		private:
			std::shared_ptr<Renderer> m_Renderer = nullptr;
			std::shared_ptr<ShaderProgram> m_Shader = nullptr;
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
			std::shared_ptr<ShaderProgram> ShadowMapShader = nullptr;
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
			void SetCastingShadows(const bool bCastShadows);

			void Apply(
				const std::shared_ptr<ShaderProgram>& shader,
				const std::shared_ptr<const Camera>& camera
			) const;

			void BeginShadowMap(
				const std::shared_ptr<Renderer>& renderer,
				const std::shared_ptr<Camera>& camera
			);
			void DrawToShadowMap(
				const std::shared_ptr<const StaticMesh>& mesh,
				const Mat4& transform
			) const;
			void EndShadowMap();

			void BeginShadowMapFar(
				const std::shared_ptr<Renderer>& renderer,
				const std::shared_ptr<Camera>& camera
			) const;
			void DrawToShadowMapFar(
				const std::shared_ptr<const StaticMesh>& mesh,
				const Mat4& transform
			) const;
			void EndShadowMapFar();

		private:
			auto CalculateLightSpace(
				const std::shared_ptr<const Camera>& camera
			) const->Mat4;
			auto CalculateLightSpaceFar(
				const std::shared_ptr<const Camera>& camera
			) const->Mat4;
			void RefreshShadowMaps();

		private:
			Vec3 m_Direction;
			Vec3 m_Color;
			float m_Intensity;
			bool m_bCastShadows;
			std::shared_ptr<ShaderProgram> m_ShadowMapShader = nullptr;
			std::unique_ptr<ShadowMap> m_ShadowMap = nullptr;
			std::unique_ptr<ShadowMap> m_ShadowMapFar = nullptr;
		};
	}
}
