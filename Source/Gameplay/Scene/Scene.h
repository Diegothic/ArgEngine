#pragma once

#include <unordered_map>
#include <string>

#include "GameObject.h"
#include "Memory/SmartPtr.h"
#include "Renderer/Renderer.h"
#include "Time/GameTime.h"

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

		template<typename TComponentSubclass>
		uint64_t CreateComponent(uint64_t ownerID);

	protected:
		void RemoveGameObject(uint64_t ID);

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
