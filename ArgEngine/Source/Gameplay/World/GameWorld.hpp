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

			auto GetRootActor() const->const std::shared_ptr<Actor>&;
			auto GetActor(const GUID& actorID) const->const std::shared_ptr<Actor>&;
			auto GetActor(const GUID& actorID) -> std::shared_ptr<Actor>&;

			auto CreateActor() -> GUID;
			auto CreateActor(const std::shared_ptr<Actor>& parentActor) -> GUID;
			void DestroyActor(const std::shared_ptr<Actor>& actor);

			void Tick(const GameTime& gameTime);
			void Render(Renderer::RenderContext& context);

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

			std::shared_ptr<Actor> m_pRootActor = nullptr;
			std::vector<std::shared_ptr<Actor>> m_pActors;
			std::unordered_map<GUID, std::shared_ptr<Actor>> m_pActorsRegistry;

			Content::ResourceCache* m_pResources = nullptr;
			ComponentRegistry* m_pComponents = nullptr;

			Random::Random m_IDGenerator;
		};
	}
}
