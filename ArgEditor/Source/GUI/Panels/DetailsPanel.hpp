#pragma once

#include <arg_pch.hpp>

#include "GUI/EditorGUIPanel.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/PointLightComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/SkeletalModelComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/SpotLightComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/StaticModelComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Physics/PhysicsBodyComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Sound/SoundPlayerComponent.hpp"
#include "Script/ScriptComponent.hpp"

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
					Gameplay::Actor* actor
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Gameplay::StaticModelComponent>& pComponent
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Gameplay::SkeletalModelComponent>& pComponent
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Gameplay::PointLightComponent>& pComponent
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Gameplay::SpotLightComponent>& pComponent
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Gameplay::CameraComponent>& pComponent
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Gameplay::PhysicsBodyComponent>& pComponent
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Gameplay::SoundPlayerComponent>& pComponent
				);

				void DrawActorComponentProperties(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent
				);

			private:
				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const float& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const int32_t& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const bool& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const Vec3& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const Gameplay::ActorHandle& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const TextureHandle& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const StaticModelHandle& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const SkeletonHandle& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const SkeletalModelHandle& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const SkeletalAnimationHandle& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const MaterialHandle& propertyValue
				);

				void DrawScriptComponentProperty(
					const EditorGUIContext& context,
					Gameplay::Actor* pActor,
					std::shared_ptr<Script::ScriptComponent>& pComponent,
					const std::string& propertyName,
					const SoundResourceHandle& propertyValue
				);
			};
		}
	}
}
