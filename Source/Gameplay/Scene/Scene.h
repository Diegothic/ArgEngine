#pragma once

#include <unordered_map>
#include <string>

#include "Arg/Time.h"
#include "Arg/Renderer.h"

namespace Arg
{
	class GameObject;

	class Scene
	{
	public:
		Scene()
		{
		}
		~Scene() = default;

		void Start();
		void Tick(double deltaTime);
		void Render(Box<Renderer>& renderer);

		GameObject* GetRootObject() const;
		GameObject* FindGameObject(uint64_t ID);

		uint64_t CreateGameObject(const std::string& name, uint64_t parentID = 0);
		void DestroyGameObject(uint64_t ID);
		void ChangeParentGameObject(uint64_t ID, uint64_t parentID = 0);

	private:
		Box<GameObject> m_RootObject;
		std::vector<Rc<GameObject>> m_GameObjects;
		std::unordered_map<uint64_t, GameObject*> m_GameObjectsRegistry;
		GameTime m_GameTime;

		// TEMP:
		uint64_t m_LastUsedID = 1;

		std::string vertexSource;
		std::string fragmentSource;
		uint32_t shader = 0;
		Vec3 cameraPosition = Vec3(0.0f, 0.0f, -5.0f);
		Vec3 cameraForward = Vec3(0.0f, 0.0f, 1.0f);
		Vec3 cameraUp = Vec3(0.0f, -1.0f, 0.0f);
		uint32_t quadVertexArray = 0;
	};
}
