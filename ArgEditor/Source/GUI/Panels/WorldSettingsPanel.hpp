#pragma once

#include <arg_pch.hpp>

#include "GUI/EditorGUIPanel.hpp"

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			class WorldSettingsPanel : public EditorGUIPanel
			{
			public:
				void OnInitialize(const EditorGUIContext& context) override;
				void OnDraw(const EditorGUIContext& context) override;
			};
		}
	}
}
