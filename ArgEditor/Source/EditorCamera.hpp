#pragma once

#include <arg_pch.hpp>

#include "Core/Window/Window.hpp"
#include "Renderer/Camera/Camera.hpp"
#include "Core/Input/Input.hpp"

namespace Arg
{
	namespace Editor
	{
		class EditorCamera
		{
		public:
			EditorCamera(Window* window);

			auto GetView() const->Mat4;
			auto GetProjection(const float aspectRatio) const->Mat4;

			auto GetCamera() const -> const std::shared_ptr<Renderer::Camera>& { return m_pCamera; }

			auto GetSpeed() const -> const float& { return m_Speed; }
			void SetSpeed(const float speed) { m_Speed = speed; }

			void Update(
				const float deltaTime,
				const Input::Input& input
			);

			void Reset();
			void Cancel();

		private:
			Window* m_pWindow = nullptr;
			std::shared_ptr<Renderer::Camera> m_pCamera = nullptr;
			bool m_bLookWithMouse = false;
			float m_Speed = 1.0f;
		};
	}
}
