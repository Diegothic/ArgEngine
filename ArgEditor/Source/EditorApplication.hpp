#pragma once

#include <arg_pch.hpp>

#include "Core/Application.hpp"
#include "Core/Window/Window.hpp"

#include "Editor.hpp"

namespace Arg
{
	class EditorApplication : public Application
	{
	public:
		EditorApplication() = default;
		~EditorApplication() override = default;

	protected:
		auto VOnStartUp() -> void override;
		auto VOnShutDown() -> void override;
		auto VOnRun() -> void override;

	private:
		std::shared_ptr<Window> m_pWindow = nullptr;
		std::unique_ptr<Editor::Editor> m_pEditor = nullptr;
	};
}
