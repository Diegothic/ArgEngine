#include <arg_pch.hpp>
#include "GameWorld.hpp"

#include "Debug/Assert.hpp"
#include "GameEngine.hpp"

Arg::GameEngine* Arg::Gameplay::GameWorld::s_pEngine = nullptr;

Arg::Gameplay::GameWorld::GameWorld(Content::Resource* pResource)
{
	m_pResource = pResource;
	m_pRootActor = std::make_unique<Actor>(GUID::Empty, this);
}

void Arg::Gameplay::GameWorld::Create()
{
	s_pEngine->InitializeWorld(this);
}

void Arg::Gameplay::GameWorld::Initialize(const GameContext& context)
{
	m_pComponents = context.Components;
}

auto Arg::Gameplay::GameWorld::GetRootActor() const -> Actor&
{
	return *m_pRootActor;
}

auto Arg::Gameplay::GameWorld::GetActor(const GUID& actorID) const -> const Actor&
{
	ARG_ASSERT(m_ActorsRegistry.contains(actorID), "Invalid Actor ID!");
	return *m_ActorsRegistry.at(actorID);
}

auto Arg::Gameplay::GameWorld::GetActor(const GUID& actorID) -> Actor&
{
	ARG_ASSERT(m_ActorsRegistry.contains(actorID), "Invalid Actor ID!");
	return *m_ActorsRegistry.at(actorID);
}

auto Arg::Gameplay::GameWorld::CreateActor() -> GUID
{
	return CreateActor(*m_pRootActor);
}

auto Arg::Gameplay::GameWorld::CreateActor(Actor& parentActor) -> GUID
{
	const GUID newActorID = GenerateID();
	const auto& newActor = m_Actors.emplace_back(std::make_unique<Actor>(newActorID, this));
	m_ActorsRegistry[newActorID] = newActor.get();

	newActor->SetParentActor(&parentActor);
	parentActor.AddChildActor(newActor.get());

	return newActorID;
}

void Arg::Gameplay::GameWorld::DestroyActor(Actor& actor)
{
	Actor* parentActor = actor.GetParentActor();
	for (size_t i = actor.GetChildActorsCount(); i > 0; i--)
	{
		ReparentActor(*actor.GetChildActor(i - 1), *parentActor);
	}

	parentActor->RemoveChildActor(&actor);
	actor.Destroy();

	const GUID actorID = actor.GetID();
	ARG_ASSERT(m_ActorsRegistry.contains(actorID), "Invalid Actor ID!");
	m_ActorsRegistry.erase(actorID);

	const auto it = std::ranges::find_if(m_Actors, [&](const std::unique_ptr<Actor>& pActor)
	{
		return pActor->GetID() == actorID;
	});
	ARG_ASSERT(it != m_Actors.end(), "Invalid Actor!");
	m_Actors.erase(it);
}

void Arg::Gameplay::GameWorld::ReparentActor(Actor& actor, Actor& newParentActor)
{
	actor.GetParentActor()->RemoveChildActor(&actor);
	actor.SetParentActor(&newParentActor);
	actor.ReparentTransform(newParentActor);
	newParentActor.AddChildActor(&actor);
}

void Arg::Gameplay::GameWorld::Tick(const GameTime& gameTime)
{
	for (const auto& actor : m_Actors)
	{
		actor->Tick(gameTime);
	}
}

void Arg::Gameplay::GameWorld::Render(Renderer::RenderContext& context)
{
	const auto& rootActor = GetRootActor();
	for (size_t i = 0; i < rootActor.GetChildActorsCount(); i++)
	{
		const Mat4& rootActorTransform = rootActor.GetTransform();
		Actor* childActor = rootActor.GetChildActor(i);
		childActor->UpdateTransform(rootActorTransform);
	}

	for (const auto& actor : m_Actors)
	{
		actor->Render(context);
	}
}

void Arg::Gameplay::GameWorld::ClearGarbage()
{
	std::vector<GUID> actorsToDestroy;
	actorsToDestroy.reserve(m_Actors.size());
	for (const auto& actor : m_Actors)
	{
		if (actor->IsMarkedForDestruction())
		{
			actorsToDestroy.push_back(actor->GetID());
		}
	}

	for (const auto& actorID : actorsToDestroy)
	{
		Actor& actor = GetActor(actorID);
		DestroyActor(actor);
	}
}

auto Arg::Gameplay::GameWorld::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["World"];

	header["LastGeneratedID"] = m_IDGenerator.GetSeed();

	auto actorsNode = header["Actors"];
	actorsNode.reset();
	for (const auto& actor : m_Actors)
	{
		YAML::Node actorNode;
		if (actor->Serialize(actorNode))
		{
			actorsNode.push_back(actorNode);
		}
	}

	header["Actors"] = actorsNode;

	auto actorRelationsNode = header["ActorRelations"];
	actorRelationsNode.reset();
	for (size_t i = 0; i < m_pRootActor->GetChildActorsCount(); i++)
	{
		YAML::Node relationNode;
		relationNode["ParentID"] = m_pRootActor->GetID();
		relationNode["ChildID"] = m_pRootActor->GetChildActor(i)->GetID();
		actorRelationsNode.push_back(relationNode);
	}

	for (const auto& actor : m_Actors)
	{
		for (size_t i = 0; i < actor->GetChildActorsCount(); i++)
		{
			YAML::Node relationNode;
			relationNode["ParentID"] = actor->GetID();
			relationNode["ChildID"] = actor->GetChildActor(i)->GetID();
			actorRelationsNode.push_back(relationNode);
		}
	}

	header["ActorRelations"] = actorRelationsNode;

	return true;
}

auto Arg::Gameplay::GameWorld::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto& header = node["World"];
	if (!header)
	{
		return false;
	}

	const auto lastGeneratedID = ValueOr<uint64_t>(header["LastGeneratedID"], 0);
	m_IDGenerator.SetSeed(lastGeneratedID);

	m_ActorsRegistry.clear();
	m_Actors.clear();
	m_pRootActor->ClearChildActors();
	const auto& actorsNode = header["Actors"];
	if (actorsNode)
	{
		for (size_t i = 0; i < actorsNode.size(); i++)
		{
			const auto& actorNode = actorsNode[i];
			auto& actor = m_Actors.emplace_back(std::make_unique<Actor>(GUID::Empty, this));
			actor->Deserialize(actorNode);

			if (actor->GetID() == GUID::Empty)
			{
				actor->SetID(GenerateID());
			}
			m_ActorsRegistry[actor->GetID()] = actor.get();

			actor->SetParentActor(m_pRootActor.get());
			m_pRootActor->AddChildActor(actor.get());
		}
	}

	const auto& actorsRelationsNode = header["ActorRelations"];
	if (actorsRelationsNode)
	{
		for (size_t i = 0; i < actorsRelationsNode.size(); i++)
		{
			const auto& relationNode = actorsRelationsNode[i];
			const GUID parentID = ValueOr<GUID>(relationNode["ParentID"], GUID::Empty);
			const GUID childID = ValueOr<GUID>(relationNode["ChildID"], GUID::Empty);
			if (childID == GUID::Empty)
			{
				ARG_CONSOLE_LOG_WARN("Invalid actor relation [%llu] for map %s", i, GetName().c_str());
				continue;
			}

			Actor* childActor = m_ActorsRegistry.at(childID);
			Actor* parentActor = parentID == GUID::Empty ? m_pRootActor.get() : m_ActorsRegistry.at(parentID);
			childActor->GetParentActor()->RemoveChildActor(childActor);
			childActor->SetParentActor(parentActor);
			parentActor->AddChildActor(childActor);
		}
	}

	return true;
}

auto Arg::Gameplay::GameWorld::GenerateID() -> GUID
{
	const GUID ID = GUID(m_IDGenerator.Next() + static_cast<uint64_t>(1));
	return ID;
}
