#include <arg_pch.hpp>
#include "GameWorld.hpp"

#include "Debug/Assert.hpp"
#include "GameEngine.hpp"

Arg::GameEngine* Arg::Gameplay::GameWorld::s_pEngine = nullptr;

Arg::Gameplay::GameWorld::GameWorld()
{
	m_pRootActor = std::make_shared<Actor>(GUID::Empty, this);
}

void Arg::Gameplay::GameWorld::Create()
{
	s_pEngine->InitializeWorld(this);
}

void Arg::Gameplay::GameWorld::Initialize(const GameContext& context)
{
	m_pResources = context.Resources;
	m_pComponents = context.Components;
}

auto Arg::Gameplay::GameWorld::GetRootActor() const->const std::shared_ptr<Actor>&
{
	return m_pRootActor;
}

auto Arg::Gameplay::GameWorld::GetActor(const GUID& actorID) const->const std::shared_ptr<Actor>&
{
	ARG_ASSERT(m_pActorsRegistry.contains(actorID), "Invalid Actor ID!");
	return m_pActorsRegistry.at(actorID);
}

auto Arg::Gameplay::GameWorld::GetActor(const GUID& actorID) ->std::shared_ptr<Actor>&
{
	ARG_ASSERT(m_pActorsRegistry.contains(actorID), "Invalid Actor ID!");
	return m_pActorsRegistry.at(actorID);
}

auto Arg::Gameplay::GameWorld::CreateActor() -> GUID
{
	return GUID::Empty;
}

auto Arg::Gameplay::GameWorld::CreateActor(const std::shared_ptr<Actor>& parentActor) -> GUID
{
	return GUID::Empty;
}

void Arg::Gameplay::GameWorld::DestroyActor(const std::shared_ptr<Actor>& actor)
{

}

void Arg::Gameplay::GameWorld::Tick(const GameTime& gameTime)
{
	for (auto& actor : m_pActors)
	{
		actor->Tick(gameTime);
	}
}

void Arg::Gameplay::GameWorld::Render(Renderer::RenderContext& context)
{
	auto& rootActor = GetRootActor();
	for (size_t i = 0; i < rootActor->GetChildActorsCount(); i++)
	{
		const Mat4& rootActorTransform = rootActor->GetTransform();
		const auto& childActor = rootActor->GetChildActor(i);
		childActor->UpdateTransform(rootActorTransform);
	}

	for (auto& actor : m_pActors)
	{
		actor->Render(context);
	}
}

auto Arg::Gameplay::GameWorld::VOnSerialize(YAML::Node& node) const -> bool
{
	return true;
}

auto Arg::Gameplay::GameWorld::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto& header = node["World"];
	if (!header)
	{
		return false;
	}

	m_Name = ValueOr<std::string>(header["Name"], "Default");

	const auto lastGeneratedID = ValueOr<uint64_t>(header["LastGeneratedID"], 0);
	m_IDGenerator.SetSeed(lastGeneratedID);

	m_pActorsRegistry.clear();
	m_pRootActor->ClearChildActors();
	const auto& actors = header["Actors"];
	if (actors)
	{
		for (size_t i = 0; i < actors.size(); i++)
		{
			auto actor = std::make_shared<Actor>(GUID::Empty, this);
			const auto& actorNode = actors[i];
			actor->Deserialize(actorNode);
			m_pActors.push_back(actor);

			if (actor->GetID() == GUID::Empty)
			{
				actor->SetID(GenerateID());
			}
			m_pActorsRegistry[actor->GetID()] = actor;

			actor->SetParentActor(m_pRootActor.get());
			m_pRootActor->AddChildActor(actor);
		}
	}

	return true;
}

auto Arg::Gameplay::GameWorld::GenerateID() ->GUID
{
	const GUID ID = GUID(m_IDGenerator.Next() + static_cast<uint64_t>(1));
	return ID;
}
