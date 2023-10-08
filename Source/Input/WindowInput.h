#pragma once

#include "Input/KeyboardState.h"
#include "Input/MouseButtons.h"
#include "Input/MouseState.h"
#include "Input/GamepadState.h"

#include "Arg/Memory.h"

struct GLFWwindow;

namespace Arg
{
	class WindowInput
	{
	public:
		WindowInput();
		~WindowInput();

		void Initialize(GLFWwindow* windowHandle);
		void PrePullEvents() const;
		void PostPullEvents() const;

		const Rc<KeyboardState>& GetKeyboardState() const;
		const Rc<MouseState>& GetMouseState() const;
		const Rc<GamepadState>& GetGamepadState(int id) const;

	private:
		void CreateKeyboardState();
		void CreateMouseState();
		void CreateGamepadStateForID(int id);
		void RemoveGamepadStateForID(int id);

	private:
		void OnKeyPressed(int key, int mods) const;
		void OnKeyReleased(int key) const;
		void OnMouseButtonPressed(int button, int mods) const;
		void OnMouseButtonReleased(int button) const;
		void OnMousePositionChanged(Vec2 position) const;
		void OnMouseScrollChanged(double scroll) const;
		void OnGamepadConnected(int id);
		void OnGamepadDisconnected(int id);

	private:
		static std::map<GLFWwindow*, WindowInput*> s_WindowInputRegistry;
		static void InputKeyCallback(
			GLFWwindow* windowHandle,
			int key,
			int scanCode,
			int action,
			int mods
		);
		static void InputMouseButtonCallback(
			GLFWwindow* windowHandle,
			int button,
			int action,
			int mods
		);
		static void InputMousePositionCallback(
			GLFWwindow* windowHandle,
			double posX,
			double posY
		);
		static void InputMouseScrollCallback(
			GLFWwindow* windowHandle,
			double horizontal,
			double vertical
		);
		static void InputGamepadCallback(
			int id,
			int event
		);

	private:
		GLFWwindow* m_pWindowHandle;
		Rc<KeyboardState> m_pKeyboardState;
		Rc<MouseState> m_pMouseState;
		std::map<int, Rc<GamepadState>> m_pGamepadStateForID;
	};
}
