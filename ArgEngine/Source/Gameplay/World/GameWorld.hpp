#pragma once

#include <arg_pch.hpp>

#include "Content/Serialization/YamlSerializable.hpp"
#include "Actor/Actor.hpp"
#include "Actor/Component/ActorComponentHandle.hpp"
#include "Actor/Component/Components/Graphics/CameraComponent.hpp"
#include "Gameplay/GameContext.hpp"
#include "Physics/PhysicsWorld.hpp"
#include "Renderer/RenderContext.hpp"
#include "Sound/SoundEngine.hpp"

namespace Arg
{
	class GameEngine;

	namespace Gameplay
	{
		class GameWorld : public Content::YamlSerializable
		{
		public:
			static void SetGameEngine(GameEngine* pEngine) { s_pEngine = pEngine; }

		public:
			GameWorld(Content::Resource* pResource);
			GameWorld(const GameWorld&) = delete;
			virtual ~GameWorld() = default;

			void Create();
			void Initialize(const GameContext& context);
			void Destroy();

			auto GetName() const -> const std::string& { return m_pResource->GetName(); }

		public:
			auto HasActor(const GUID& actorID) const -> bool;
			auto GetRootActor() const -> Actor&;
			auto GetActor(const GUID& actorID) const -> const Actor&;
			auto GetActor(const GUID& actorID) -> Actor&;

			auto CreateActor() -> GUID;
			auto CreateActor(Actor& parentActor) -> GUID;
			void DestroyActor(Actor& actor);
			void ReparentActor(Actor& actor, Actor& newParentActor);

		public:
			auto GetSunlightColor() const -> Vec3;
			void SetSunlightColor(const Vec3& color);
			auto GetSunlightDirection() const -> Vec3;
			void SetSunlightDirection(const Vec3& direction);
			auto GetSunlightIntensity() const -> float;
			void SetSunlightIntensity(float intensity);
			auto GetSunlightCastsShadows() const -> bool;
			void SetSunlightCastsShadows(bool bCastShadows);

		public:
			auto GetUsingSkybox() const -> bool;
			void SetUsingSkybox(bool bUsingSkybox);
			auto GetBackgroundColor() const -> Vec3;
			void SetBackgroundColor(const Vec3& color);
			auto GetSkyboxTexture(size_t index) const -> TextureHandle;
			void SetSkyboxTexture(size_t index, const TextureHandle& texture);

		public:
			auto GetGravity() const -> const Vec3&;
			void SetGravity(const Vec3& gravity);

		public:
			auto GetMainCamera() const -> const ActorComponentHandle<CameraComponent>&
			{
				return m_MainCamera;
			}

			void SetMainCamera(const ActorComponentHandle<CameraComponent>& camera)
			{
				m_MainCamera = camera;
			}

		public:
			auto GetPhysicsWorld() const -> Physics::PhysicsWorld* { return m_pPhysicsWorld.get(); }
			auto GetSound() const -> Sound::SoundEngine&;

		public:
			void BeginPlay();
			void EndPlay();
			void Tick(const GameTime& gameTime);
			void Render(Renderer::RenderContext& context);
			void ClearGarbage();

			auto GetResourceCache() const -> Content::ResourceCache* { return m_pResource->GetResourceCache(); }
			auto GetComponentRegistry() const -> ComponentRegistry* { return m_pComponents; }

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			void CheckSkybox();

		private:
			auto GenerateID() -> GUID;

		private:
			static GameEngine* s_pEngine;

		private:
			Content::Resource* m_pResource = nullptr;
			ComponentRegistry* m_pComponents = nullptr;

			std::unique_ptr<Actor> m_pRootActor = nullptr;
			std::vector<std::unique_ptr<Actor>> m_Actors;
			std::unordered_map<GUID, Actor*> m_ActorsRegistry;

			std::unique_ptr<Renderer::DirectionalLight> m_pSunlight = nullptr;
			Vec3 m_BackgroundColor = Vec3(0.5f);
			std::vector<TextureHandle> m_SkyboxTextures;
			std::unique_ptr<Renderer::CubeMap> m_pSkybox = nullptr;
			bool m_bUsingSkybox = false;
			bool m_bSkyboxValid = false;

			ActorComponentHandle<CameraComponent> m_MainCamera;
			std::unique_ptr<Physics::PhysicsWorld> m_pPhysicsWorld = nullptr;
			Vec3 m_Gravity = Vec3(0.0f, 0.0f, -9.81f);

			Random::Random m_IDGenerator;
		};
	}
}
