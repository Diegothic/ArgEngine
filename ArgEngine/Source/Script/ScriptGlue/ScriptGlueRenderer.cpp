#include <arg_pch.hpp>
#include "ScriptGlueRenderer.hpp"

void Arg::Script::ScriptExport_Renderer(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<Renderer::RenderContext>(
		"RenderContext",
		"DrawLine",
		sol::overload(
			[](
			Renderer::RenderContext& self,
			const Vec3& from,
			const Vec3& to,
			const Vec3& color
		)
			{
				self.DrawDebugLine(
					from,
					to,
					color
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& from,
			const Vec3& to
		)
			{
				self.DrawDebugLine(
					from,
					to,
					Renderer::DEBUG_COLOR_USER
				);
			}
		),
		"DrawBox",
		sol::overload(
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const Vec3& size,
			const Vec3& color
		)
			{
				self.DrawDebugBox(
					position,
					rotation,
					size,
					color
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const Vec3& size
		)
			{
				self.DrawDebugBox(
					position,
					rotation,
					size,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation
		)
			{
				self.DrawDebugBox(
					position,
					rotation,
					Math::VEC3_ONE,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position
		)
			{
				self.DrawDebugBox(
					position,
					Math::VEC3_ZERO,
					Math::VEC3_ONE,
					Renderer::DEBUG_COLOR_USER
				);
			}
		),
		"DrawSphere",
		sol::overload(
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const float& radius,
			const Vec3& color
		)
			{
				self.DrawDebugSphere(
					position,
					rotation,
					radius,
					color
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const float& radius
		)
			{
				self.DrawDebugSphere(
					position,
					rotation,
					radius,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation
		)
			{
				self.DrawDebugSphere(
					position,
					rotation,
					1.0f,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position
		)
			{
				self.DrawDebugSphere(
					position,
					Math::VEC3_ZERO,
					1.0f,
					Renderer::DEBUG_COLOR_USER
				);
			}
		),
		"DrawCylinder",
		sol::overload(
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const float& radius,
			const float& height,
			const Vec3& color
		)
			{
				self.DrawDebugCylinder(
					position,
					rotation,
					radius,
					height,
					color
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const float& radius,
			const float& height
		)
			{
				self.DrawDebugCylinder(
					position,
					rotation,
					radius,
					height,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation
		)
			{
				self.DrawDebugCylinder(
					position,
					rotation,
					1.0f,
					1.0f,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position
		)
			{
				self.DrawDebugCylinder(
					position,
					Math::VEC3_ZERO,
					1.0f,
					1.0f,
					Renderer::DEBUG_COLOR_USER
				);
			}
		),
		"DrawCapsule",
		sol::overload(
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const float& radius,
			const float& height,
			const Vec3& color
		)
			{
				self.DrawDebugCapsule(
					position,
					rotation,
					radius,
					height,
					color
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation,
			const float& radius,
			const float& height
		)
			{
				self.DrawDebugCapsule(
					position,
					rotation,
					radius,
					height,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position,
			const Vec3& rotation
		)
			{
				self.DrawDebugCapsule(
					position,
					rotation,
					1.0f,
					1.0f,
					Renderer::DEBUG_COLOR_USER
				);
			},
			[](
			Renderer::RenderContext& self,
			const Vec3& position
		)
			{
				self.DrawDebugCapsule(
					position,
					Math::VEC3_ZERO,
					1.0f,
					1.0f,
					Renderer::DEBUG_COLOR_USER
				);
			}
		)
	);
}
