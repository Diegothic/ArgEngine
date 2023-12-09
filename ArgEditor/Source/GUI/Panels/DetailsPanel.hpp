#pragma once

#include <arg_pch.hpp>

#include "GUI/EditorGUIPanel.hpp"
#include "Content/Content.hpp"
#include "Gameplay/World/Actor/Actor.hpp"
#include "Gameplay/World/Actor/Component/Components/StaticModelComponent.hpp"

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			class DetailsPanel : public EditorGUIPanel
			{
			public:
				void OnInitialize(const EditorGUIContext& context) override;
				void OnDraw(const EditorGUIContext& context) override;

			private:
				void DrawActorDetails(
					const EditorGUIContext& context,
					std::shared_ptr<Gameplay::Actor>& actor
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					std::shared_ptr<Gameplay::Actor>& actor,
					std::shared_ptr<Gameplay::StaticModelComponent>& component
				);
			};
		}
	}
}
