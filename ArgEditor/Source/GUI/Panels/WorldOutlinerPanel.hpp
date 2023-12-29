#pragma once

#include <arg_pch.hpp>

#include "GUI/EditorGUIPanel.hpp"
#include "Content/Content.hpp"
#include "Gameplay/World/GameWorld.hpp"
#include "Content/Resource/GameResources/TextureResource.hpp"

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			class WorldOutlinerPanel : public EditorGUIPanel
			{
			public:
				void OnInitialize(const EditorGUIContext& context) override;
				void OnDraw(const EditorGUIContext& context) override;
				void DrawActorTree(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					const int32_t treeLevel
				);

			private:
				Content::ResourceHandle<Content::TextureResource> m_WorldTexture;
				Content::ResourceHandle<Content::TextureResource> m_ActorTexture;
				bool m_bHasClickedActor;
			};
		}
	}
}
