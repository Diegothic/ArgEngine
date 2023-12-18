#pragma once

#include <arg_pch.hpp>

#include "Content/Serialization/YamlSerializable.hpp"
#include "Actor/Actor.hpp"
#include "Gameplay/GameContext.hpp"
#include "Renderer/RenderContext.hpp"

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
			GameWorld();

			void Create();
			void Initialize(const GameContext& context);

			auto GetName() const -> const std::string& { return m_Name; }

		public:
			auto GetRootActor() const -> Actor&;
			auto GetActor(const GUID& actorID) const -> const Actor&;
			auto GetActor(const GUID& actorID) -> Actor&;

			auto CreateActor() -> GUID;
			auto CreateActor(Actor& parentActor) -> GUID;
			void DestroyActor(Actor& actor);
			void ReparentActor(Actor& actor, Actor& newParentActor);

		public:
			void Tick(const GameTime& gameTime);
			void Render(Renderer::RenderContext& context);
			void ClearGarbage();

			auto GetResourceCache() const -> Content::ResourceCache* { return m_pResources; }
			auto GetComponentRegistry() const -> ComponentRegistry* { return m_pComponents; }

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			auto GenerateID() -> GUID;

		private:
			static GameEngine* s_pEngine;

		private:
			std::string m_Name = "Default";

			std::unique_ptr<Actor> m_pRootActor = nullptr;
			std::vector<std::unique_ptr<Actor>> m_Actors;
			std::unordered_map<GUID, Actor*> m_ActorsRegistry;

			Content::ResourceCache* m_pResources = nullptr;
			ComponentRegistry* m_pComponents = nullptr;

			Random::Random m_IDGenerator;
		};
	}
}