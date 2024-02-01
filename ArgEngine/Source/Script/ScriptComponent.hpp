#pragma once

#include <arg_pch.hpp>

#include <sol/sol.hpp>

#include "Content/Resource/GameResources/MaterialResource.hpp"
#include "Content/Resource/GameResources/SkeletalAnimationResource.hpp"
#include "Content/Resource/GameResources/SkeletalModelResource.hpp"
#include "Content/Resource/GameResources/SkeletonResource.hpp"
#include "Content/Resource/GameResources/StaticModelResource.hpp"
#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponent.hpp"

namespace Arg
{
	using TextureHandle = Content::ResourceHandle<Content::TextureResource>;
	using StaticModelHandle = Content::ResourceHandle<Content::StaticModelResource>;
	using SkeletonHandle = Content::ResourceHandle<Content::SkeletonResource>;
	using SkeletalModelHandle = Content::ResourceHandle<Content::SkeletalModelResource>;
	using SkeletalAnimationHandle = Content::ResourceHandle<Content::SkeletalAnimationResource>;
	using MaterialHandle = Content::ResourceHandle<Content::MaterialResource>;
	using SoundResourceHandle = Content::ResourceHandle<Content::SoundResource>;

	namespace Script
	{
		class ScriptEngine;

		class ScriptComponent : public Gameplay::ActorComponent
		{
		public:
			enum ScriptComponentFieldType
			{
				FieldFloat = 0,
				FieldInteger = 1,
				FieldBoolean = 2,
				FieldVec3 = 10,

				FieldActor = 20,
				FieldActorComponent = 21,

				FieldTexture = 100,
				FieldStaticModel = 101,
				FieldSkeleton = 102,
				FieldSkeletalModel = 103,
				FieldSkeletalAnimation = 104,
				FieldMaterial = 105,
				FieldSound = 106
			};

			struct ScriptComponentField
			{
				std::string Name;
				ScriptComponentFieldType Type;
			};

		public:
			ScriptComponent(
				ScriptEngine* pScriptEngine,
				const std::string& name
			);

			ScriptComponent(ScriptComponent* pBase);

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override;
			auto VGetName() const -> const std::string& override;

			void VBeginPlay() override;
			void VTick(const Gameplay::GameTime& gameTime) override;
			void VDrawDebug(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

		public:
			auto GetFieldsCount() const -> size_t { return m_pBase->m_Fields.size(); }
			auto GetField(size_t index) const -> const ScriptComponentField&;

			void SetFieldValue(const std::string& name, float value);
			void GetFieldValue(const std::string& name, float& outValue) const;
			void DefineField(const std::string& name, const float& defaultValue);

			void SetFieldValue(const std::string& name, int32_t value);
			void GetFieldValue(const std::string& name, int32_t& outValue) const;
			void DefineField(const std::string& name, const int32_t& defaultValue);

			void SetFieldValue(const std::string& name, bool value);
			void GetFieldValue(const std::string& name, bool& outValue) const;
			void DefineField(const std::string& name, const bool& defaultValue);

			void SetFieldValue(const std::string& name, Vec3 value);
			void GetFieldValue(const std::string& name, Vec3& outValue) const;
			void DefineField(const std::string& name, const Vec3& defaultValue);

			void SetFieldValue(const std::string& name, Gameplay::ActorHandle value);
			void GetFieldValue(const std::string& name, Gameplay::ActorHandle& outValue) const;
			void DefineField(const std::string& name, const Gameplay::ActorHandle& defaultValue);

			void SetFieldValue(const std::string& name, TextureHandle value);
			void GetFieldValue(const std::string& name, TextureHandle& outValue) const;
			void DefineField(const std::string& name, const TextureHandle& defaultValue);

			void SetFieldValue(const std::string& name, StaticModelHandle value);
			void GetFieldValue(const std::string& name, StaticModelHandle& outValue) const;
			void DefineField(const std::string& name, const StaticModelHandle& defaultValue);

			void SetFieldValue(const std::string& name, SkeletonHandle value);
			void GetFieldValue(const std::string& name, SkeletonHandle& outValue) const;
			void DefineField(const std::string& name, const SkeletonHandle& defaultValue);

			void SetFieldValue(const std::string& name, SkeletalModelHandle value);
			void GetFieldValue(const std::string& name, SkeletalModelHandle& outValue) const;
			void DefineField(const std::string& name, const SkeletalModelHandle& defaultValue);

			void SetFieldValue(const std::string& name, SkeletalAnimationHandle value);
			void GetFieldValue(const std::string& name, SkeletalAnimationHandle& outValue) const;
			void DefineField(const std::string& name, const SkeletalAnimationHandle& defaultValue);

			void SetFieldValue(const std::string& name, MaterialHandle value);
			void GetFieldValue(const std::string& name, MaterialHandle& outValue) const;
			void DefineField(const std::string& name, const MaterialHandle& defaultValue);

			void SetFieldValue(const std::string& name, SoundResourceHandle value);
			void GetFieldValue(const std::string& name, SoundResourceHandle& outValue) const;
			void DefineField(const std::string& name, const SoundResourceHandle& defaultValue);

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			void UpdateScriptFields();

		private:
			ScriptEngine* m_pScriptEngine = nullptr;

			GUID m_ID = GUID::Empty;
			std::string m_Name;
			std::string m_OwnerIDString;

			ScriptComponent* m_pBase = nullptr;

			std::vector<ScriptComponentField> m_Fields;
			std::unordered_map<std::string, float> m_FloatFieldValues;
			std::unordered_map<std::string, int32_t> m_IntegerFieldValues;
			std::unordered_map<std::string, bool> m_BooleanFieldValues;

			std::unordered_map<std::string, Vec3> m_Vec3FieldValues;

			std::unordered_map<std::string, Gameplay::ActorHandle> m_ActorFieldValues;

			std::unordered_map<std::string, TextureHandle> m_TextureFieldValues;
			std::unordered_map<std::string, StaticModelHandle> m_StaticModelFieldValues;
			std::unordered_map<std::string, SkeletonHandle> m_SkeletonFieldValues;
			std::unordered_map<std::string, SkeletalModelHandle> m_SkeletalModelFieldValues;
			std::unordered_map<std::string, SkeletalAnimationHandle> m_SkeletalAnimationFieldValues;
			std::unordered_map<std::string, MaterialHandle> m_MaterialFieldValues;
			std::unordered_map<std::string, SoundResourceHandle> m_SoundFieldValues;
		};
	}
}
