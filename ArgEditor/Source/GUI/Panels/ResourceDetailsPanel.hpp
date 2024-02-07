#pragma once

#include <arg_pch.hpp>

#include "GUI/EditorGUIPanel.hpp"
#include "Content/Content.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Animation/SkeletalAnimation.hpp"

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			class ResourceDetailsPanel : public EditorGUIPanel
			{
			public:
				void OnInitialize(
					const EditorGUIContext& context
				) override;
				void OnDraw(
					const EditorGUIContext& context
				) override;

			private:
				void DrawResourceDetails(
					const EditorGUIContext& context,
					std::shared_ptr<Content::Resource>& pResource
				);

				void DrawGameResourceDetails(
					const EditorGUIContext& context,
					std::shared_ptr<Renderer::Material>& pMaterial
				);

				void DrawGameResourceDetails(
					const EditorGUIContext& context,
					Renderer::SkeletalAnimation* pAnimation
				);
			};
		}
	}
}
