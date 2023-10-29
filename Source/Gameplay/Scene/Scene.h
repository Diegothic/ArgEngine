#pragma once

#include <unordered_map>
#include <string>

#include "Core/Memory/SmartPtr.h"
#include "Core/Time/GameTime.h"
#include "Renderer/Renderer.h"
#include "GameObject/GameObject.h"

namespace Arg
{
	class Scene
	{
	public:
		Scene()
		{
		}
		~Scene() = default;

		void Start();
		void Tick(double deltaTime);
		void Render(Renderer* renderer);
		void ClearGarbage();

		GameObject* GetRootObject() const;
		GameObject* FindGameObject(uint64_t ID);

		uint64_t CreateGameObject(const std::string& name, uint64_t parentID = 0);
		void DestroyGameObject(uint64_t ID);
		void ChangeParentGameObject(uint64_t ID, uint64_t parentID = 0);

		void SetSelectedGameObject(uint64_t ID) { m_SelectedGameObjectID = ID; }

		Component* FindComponent(uint64_t ID);
		template<typename TComponentSubclass>
		uint64_t CreateComponent(uint64_t ownerID);
		void DestroyComponent(uint64_t ID);

	protected:
		void RemoveGameObject(uint64_t ID);
		void RemoveComponent(uint64_t ID);

	private:
		Box<GameObject> m_RootObject;
		std::vector<Rc<GameObject>> m_GameObjects;
		std::vector<Rc<Component>> m_Components;

		std::unordered_map<uint64_t, GameObject*> m_GameObjectsRegistry;
		std::unordered_map<uint64_t, Component*> m_ComponentsRegistry;

		uint64_t m_SelectedGameObjectID = 0;
		GameTime m_GameTime;

		// TEMP:
		uint64_t m_LastUsedID = 1;

		std::string vertexSource;
		std::string fragmentSource;
		uint32_t shader = 0;
		Vec3 cameraPosition = Vec3(5.0f, 0.0f, 0.0f);
		Vec3 cameraForward = Vec3(1.0f, 0.0f, 0.0f);
		Vec3 cameraUp = Vec3(0.0f, 0.0f, -1.0f);
		uint32_t quadVertexArray = 0;
	};
}

template <typename TComponentSubclass>
uint64_t Arg::Scene::CreateComponent(uint64_t ownerID)
{
	m_LastUsedID++;
	GameObject* owner = FindGameObject(ownerID);
	const Rc<Component> newComponent = NewRc<TComponentSubclass>(m_LastUsedID, owner);
	owner->AddComponent(newComponent.get());

	m_Components.push_back(newComponent);
	m_ComponentsRegistry[m_LastUsedID] = newComponent.get();
	return m_LastUsedID;
}
