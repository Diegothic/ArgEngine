#pragma once

#include <arg_pch.hpp>

#include "Core/Application.hpp"
#include "Core/Window/Window.hpp"

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
		std::unique_ptr<Window> m_pWindow = nullptr;
	};
}
