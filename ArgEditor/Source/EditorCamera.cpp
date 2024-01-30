#include <arg_pch.hpp>
#include "EditorCamera.hpp"

#include "Renderer/Camera/PerspectiveCamera.hpp"

Arg::Editor::EditorCamera::EditorCamera(Window* window)
	: m_pWindow(window)
{
	m_pCamera = std::make_shared<Renderer::PerspectiveCamera>();
	m_pCamera->SetPosition(Vec3(-10.0f, 0.0f, 0.0f));
}

auto Arg::Editor::EditorCamera::GetView() const -> Mat4
{
	return m_pCamera->GetView();
}

auto Arg::Editor::EditorCamera::GetProjection(const float aspectRatio) const -> Mat4
{
	return m_pCamera->VGetProjection(aspectRatio);
}

void Arg::Editor::EditorCamera::Update(
	const float deltaTime,
	const Input::Input& input
)
{
	const auto& keyboardState = input.GetKeyboardState();
	const auto& mouseState = input.GetMouseState();


	if (mouseState.IsButtonDown(Input::MouseButton::Right)
		&& !m_bLookWithMouse
	)
	{
		m_bLookWithMouse = true;
		m_pWindow->SetCursorMode(Input::CursorMode::Locked);
	}

	if (mouseState.IsButtonUp(Input::MouseButton::Right)
		&& m_bLookWithMouse
	)
	{
		m_bLookWithMouse = false;
		m_pWindow->SetCursorMode(Input::CursorMode::Normal);
	}

	if (m_bLookWithMouse)
	{
		if (keyboardState.IsKeyDown(Input::KeyCode::W))
		{
			Vec3 cameraPos = m_pCamera->GetPosition();
			cameraPos += m_pCamera->GetForwardVector() * 10.0f * m_Speed * deltaTime;
			m_pCamera->SetPosition(cameraPos);
		}

		if (keyboardState.IsKeyDown(Input::KeyCode::S))
		{
			Vec3 cameraPos = m_pCamera->GetPosition();
			cameraPos += m_pCamera->GetForwardVector() * -10.0f * m_Speed * deltaTime;
			m_pCamera->SetPosition(cameraPos);
		}

		if (keyboardState.IsKeyDown(Input::KeyCode::D))
		{
			Vec3 cameraPos = m_pCamera->GetPosition();
			cameraPos += m_pCamera->GetRightVector() * 10.0f * m_Speed * deltaTime;
			m_pCamera->SetPosition(cameraPos);
		}

		if (keyboardState.IsKeyDown(Input::KeyCode::A))
		{
			Vec3 cameraPos = m_pCamera->GetPosition();
			cameraPos += m_pCamera->GetRightVector() * -10.0f * m_Speed * deltaTime;
			m_pCamera->SetPosition(cameraPos);
		}

		if (keyboardState.IsKeyDown(Input::KeyCode::Space))
		{
			Vec3 cameraPos = m_pCamera->GetPosition();
			cameraPos += Vec3(0.0f, 0.0f, 1.0f) * 10.0f * m_Speed * deltaTime;
			m_pCamera->SetPosition(cameraPos);
		}

		if (keyboardState.IsKeyDown(Input::KeyCode::LeftShift))
		{
			Vec3 cameraPos = m_pCamera->GetPosition();
			cameraPos += Vec3(0.0f, 0.0f, 1.0f) * -10.0f * m_Speed * deltaTime;
			m_pCamera->SetPosition(cameraPos);
		}

		const Vec2 mouseDelta = mouseState.GetPositionDelta();

		float cameraPitch = m_pCamera->GetPitch();
		cameraPitch += mouseDelta.y * 10.0f * m_Speed * deltaTime;
		cameraPitch = Math::clamp(cameraPitch, -89.0f, 89.0f);
		m_pCamera->SetPitch(cameraPitch);

		float cameraYaw = m_pCamera->GetYaw();
		cameraYaw += mouseDelta.x * -10.0f * m_Speed * deltaTime;
		m_pCamera->SetYaw(cameraYaw);
	}
}

void Arg::Editor::EditorCamera::Reset()
{
	Cancel();
	m_pCamera = std::make_shared<Renderer::PerspectiveCamera>();
	m_pCamera->SetPosition(Vec3(-10.0f, 0.0f, 0.0f));
}

void Arg::Editor::EditorCamera::Cancel()
{
	m_bLookWithMouse = false;
	m_pWindow->SetCursorMode(Input::CursorMode::Normal);
}
