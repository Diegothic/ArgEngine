#pragma once

#include <arg_pch.hpp>

#include "Core/Application.hpp"
#include "Core/Window/Window.hpp"

namespace Arg
{
	class RuntimeApplication : public Application
	{
	public:
		RuntimeApplication() = default;
		~RuntimeApplication() override = default;

	protected:
		void VOnStartUp() override;
		void VOnShutDown() override;
		void VOnRun() override;

	private:
		std::unique_ptr<Window> m_pWindow = nullptr;
	};
}
