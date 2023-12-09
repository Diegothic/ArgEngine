#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			struct EditorGUIContext;

			class EditorGUIPanel
			{
			public:
				void Initialize(const EditorGUIContext& context);

				void Open();
				auto IsOpened() const -> const bool&;

				void Draw(const EditorGUIContext& context);

			protected:
				virtual void OnInitialize(const EditorGUIContext& context) = 0;
				virtual void OnDraw(const EditorGUIContext& context) = 0;

				auto GetIsOpened() -> bool&;

			private:
				bool m_bIsOpened = false;
			};
		}
	}
}
