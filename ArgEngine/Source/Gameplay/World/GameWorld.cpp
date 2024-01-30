#include <arg_pch.hpp>
#include "GameWorld.hpp"

#include "Debug/Assert.hpp"
#include "GameEngine.hpp"

Arg::GameEngine* Arg::Gameplay::GameWorld::s_pEngine = nullptr;

Arg::Gameplay::GameWorld::GameWorld(Content::Resource* pResource)
{
	m_pResource = pResource;
	m_pRootActor = std::make_unique<Actor>(GUID::Empty, this);

	m_SkyboxTextures = std::vector<TextureHandle>(6);
}

void Arg::Gameplay::GameWorld::Create()
{
	s_pEngine->InitializeWorld(this);
}

void Arg::Gameplay::GameWorld::Initialize(const GameContext& context)
{
	m_pComponents = context.Components;

	const Renderer::DirectionalLightSpec sunlightSpec
	{
		.Direction = Vec3(-0.3f, -0.4f, -1.0f),
		.Color = Vec3(1.0f),
		.Intensity = 1.0f,
		.bCastShadows = true,
		.pShadowMapShader = nullptr
	};
	m_pSunlight = std::make_unique<Renderer::DirectionalLight>(sunlightSpec);
}

void Arg::Gameplay::GameWorld::Destroy()
{
	std::vector<GUID> actorsToDestroy;
	actorsToDestroy.reserve(m_Actors.size());
	for (const auto& actor : m_Actors)
	{
		actorsToDestroy.push_back(actor->GetID());
	}

	for (const auto& actorID : actorsToDestroy)
	{
		Actor& actor = GetActor(actorID);
		DestroyActor(actor);
	}

	for (size_t i = 0; i < 6; i++)
	{
		if (m_SkyboxTextures[i].IsValid())
		{
			m_SkyboxTextures[i].FreeRef();
		}
	}

	m_pSkybox = nullptr;
}

auto Arg::Gameplay::GameWorld::HasActor(const GUID& actorID) const -> bool
{
	return m_ActorsRegistry.contains(actorID);
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

auto Arg::Gameplay::GameWorld::GetSunlightColor() const -> Vec3
{
	return m_pSunlight->GetColor();
}

void Arg::Gameplay::GameWorld::SetSunlightColor(const Vec3& color)
{
	m_pSunlight->SetColor(color);
}

auto Arg::Gameplay::GameWorld::GetSunlightDirection() const -> Vec3
{
	return m_pSunlight->GetDirection();
}

void Arg::Gameplay::GameWorld::SetSunlightDirection(const Vec3& direction)
{
	m_pSunlight->SetDirection(direction);
}

auto Arg::Gameplay::GameWorld::GetSunlightIntensity() const -> float
{
	return m_pSunlight->GetIntensity();
}

void Arg::Gameplay::GameWorld::SetSunlightIntensity(float intensity)
{
	m_pSunlight->SetIntensity(intensity);
}

auto Arg::Gameplay::GameWorld::GetSunlightCastsShadows() const -> bool
{
	return m_pSunlight->IsCastingShadows();
}

void Arg::Gameplay::GameWorld::SetSunlightCastsShadows(bool bCastShadows)
{
	m_pSunlight->SetCastingShadows(bCastShadows);
}

auto Arg::Gameplay::GameWorld::GetUsingSkybox() const -> bool
{
	return m_bUsingSkybox;
}

void Arg::Gameplay::GameWorld::SetUsingSkybox(bool bUsingSkybox)
{
	m_bUsingSkybox = bUsingSkybox;
	if (!m_bUsingSkybox)
	{
		m_pSkybox = nullptr;
		m_bSkyboxValid = false;
	}
	else
	{
		m_pSkybox = std::make_unique<Renderer::CubeMap>();
		CheckSkybox();
	}
}

auto Arg::Gameplay::GameWorld::GetBackgroundColor() const -> Vec3
{
	return m_BackgroundColor;
}

void Arg::Gameplay::GameWorld::SetBackgroundColor(const Vec3& color)
{
	m_BackgroundColor = color;
}

auto Arg::Gameplay::GameWorld::GetSkyboxTexture(size_t index) const -> TextureHandle
{
	ARG_ASSERT(index < 6, "");
	return m_SkyboxTextures[index];
}

void Arg::Gameplay::GameWorld::SetSkyboxTexture(size_t index, const TextureHandle& texture)
{
	ARG_ASSERT(index < 6, "");

	if (m_SkyboxTextures[index].IsValid())
	{
		m_SkyboxTextures[index].FreeRef();
	}

	m_SkyboxTextures[index] = texture;

	if (m_SkyboxTextures[index].IsValid())
	{
		m_SkyboxTextures[index].AddRef();
	}

	CheckSkybox();
}

auto Arg::Gameplay::GameWorld::GetGravity() const -> const Vec3&
{
	return m_Gravity;
}

void Arg::Gameplay::GameWorld::SetGravity(const Vec3& gravity)
{
	m_Gravity = gravity;
	if (m_pPhysicsWorld != nullptr)
	{
		m_pPhysicsWorld->SetGravity(m_Gravity);
	}
}

auto Arg::Gameplay::GameWorld::GetSound() const -> Sound::SoundEngine&
{
	return s_pEngine->GetSoundEngine();
}

void Arg::Gameplay::GameWorld::BeginPlay()
{
	m_pPhysicsWorld = std::make_unique<Physics::PhysicsWorld>(this);
	m_pPhysicsWorld->Initialize();
	m_pPhysicsWorld->SetGravity(m_Gravity);

	for (const auto& actor : m_Actors)
	{
		actor->BeginPlay();
	}
}

void Arg::Gameplay::GameWorld::EndPlay()
{
	if (m_pPhysicsWorld != nullptr)
	{
		m_pPhysicsWorld->CleanUp();
		m_pPhysicsWorld = nullptr;
	}
}

void Arg::Gameplay::GameWorld::Tick(const GameTime& gameTime)
{
	m_pPhysicsWorld->Tick(gameTime.GetDeltaTime());

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

	context.AddDirectionalLight(*m_pSunlight);

	context.SetBackgroundColor(m_BackgroundColor);
	if (m_bUsingSkybox && m_bSkyboxValid)
	{
		context.SetSkybox(m_pSkybox.get());
	}

	for (const auto& actor : m_Actors)
	{
		actor->Render(context);
	}
}

void Arg::Gameplay::GameWorld::DrawDebug(Renderer::RenderContext& context)
{
	GUID mainCameraOwnerID = GUID::Empty;
	if (s_pEngine->IsPlaying())
	{
		mainCameraOwnerID = GetMainCamera().GetOwnerID();
	}

	for (const auto& actor : m_Actors)
	{
		if (actor->GetID() == mainCameraOwnerID)
		{
			continue;
		}

		actor->DrawDebug(context);
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

	auto sunlightNode = header["Sunlight"];
	sunlightNode["Color"] = m_pSunlight->GetColor();
	sunlightNode["Direction"] = m_pSunlight->GetDirection();
	sunlightNode["Intensity"] = m_pSunlight->GetIntensity();
	sunlightNode["CastShadows"] = m_pSunlight->IsCastingShadows();
	header["Sunlight"] = sunlightNode;

	auto backgroundNode = header["Background"];
	backgroundNode["UsingSkybox"] = m_bUsingSkybox;
	backgroundNode["Color"] = m_BackgroundColor;
	auto backgroundTexturesNode = backgroundNode["Textures"];
	for (size_t i = 0; i < 6; i++)
	{
		backgroundTexturesNode[i] = m_SkyboxTextures[i].GetID();
	}
	backgroundNode["Textures"] = backgroundTexturesNode;
	header["Background"] = backgroundNode;


	auto gameplayNode = header["Gameplay"];
	gameplayNode["MainCamera"] = GetMainCamera().GetOwnerID();
	header["Gameplay"] = gameplayNode;

	auto physicsNode = header["Physics"];
	physicsNode["Gravity"] = m_Gravity;
	header["Physics"] = physicsNode;

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

	Content::ResourceCache* pResourceCache = m_pResource->GetResourceCache();

	const auto lastGeneratedID = ValueOr<uint64_t>(header["LastGeneratedID"], 0);
	m_IDGenerator.SetSeed(lastGeneratedID);

	const auto& sunlightNode = header["Sunlight"];
	if (sunlightNode)
	{
		m_pSunlight->SetColor(ValueOr<Vec3>(sunlightNode["Color"], Vec3(1.0f)));
		m_pSunlight->SetDirection(ValueOr<Vec3>(sunlightNode["Direction"], Vec3(0.0f, 0.0f, -1.0f)));
		m_pSunlight->SetIntensity(ValueOr<float>(sunlightNode["Intensity"], 1.0f));
		m_pSunlight->SetCastingShadows(ValueOr<bool>(sunlightNode["CastShadows"], true));
	}

	const auto& backgroundNode = header["Background"];
	if (backgroundNode)
	{
		SetBackgroundColor(ValueOr<Vec3>(backgroundNode["Color"], Vec3(1.0f)));

		const auto& backgroundTexturesNode = backgroundNode["Textures"];
		for (size_t i = 0; i < 6; i++)
		{
			const GUID textureID = ValueOr<GUID>(backgroundTexturesNode[i], GUID::Empty);
			SetSkyboxTexture(i, pResourceCache->CreateHandle<Content::TextureResource>(textureID));
		}

		SetUsingSkybox(ValueOr<bool>(backgroundNode["UsingSkybox"], false));
	}

	const auto& gameplayNode = header["Gameplay"];
	if (gameplayNode)
	{
		const GUID mainCameraActorID = ValueOr<GUID>(gameplayNode["MainCamera"], GUID::Empty);
		SetMainCamera(ActorComponentHandle<CameraComponent>(this, mainCameraActorID, CameraComponent::COMPONENT_ID));
	}

	const auto& physicsNode = header["Physics"];
	if (physicsNode)
	{
		m_Gravity = ValueOr<Vec3>(physicsNode["Gravity"], Vec3(0.0f, 0.0f, -9.81f));
	}

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

void Arg::Gameplay::GameWorld::CheckSkybox()
{
	m_bSkyboxValid = false;

	bool bAllTexturesValid = true;
	for (size_t i = 0; i < 6; i++)
	{
		if (!m_SkyboxTextures[i].IsValid())
		{
			bAllTexturesValid = false;
			break;
		}
	}

	if (bAllTexturesValid && m_bUsingSkybox)
	{
		std::vector<TextureHandle> faces(6);
		for (size_t i = 0; i < 6; i++)
		{
			faces[i] = m_SkyboxTextures[i];
		}
		m_pSkybox->SetFaces(faces);

		m_bSkyboxValid = true;
	}
}

auto Arg::Gameplay::GameWorld::GenerateID() -> GUID
{
	const GUID ID = GUID(m_IDGenerator.Next() + static_cast<uint64_t>(1));
	return ID;
}
