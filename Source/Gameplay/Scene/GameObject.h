#pragma once

#include <string>
#include <vector>

#include "Components/Component.h"
#include "Components/Transform.h"
#include "Renderer/RenderContext.h"
#include "Time/GameTime.h"

namespace Arg
{
	class Scene;

	class GameObject
	{
	public:
		GameObject(uint64_t id, Scene* scene, std::string name = std::string());
		~GameObject() = default;
		bool operator==(const GameObject& other) const;

		uint64_t GetID() const { return m_ID; }
		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		Scene* GetScene() const;

		void Tick(const GameTime& gameTime) const;
		void Render(const RenderContext& renderContext) const;
		void PrepareForRender();
		void Destroy();

		bool IsMarkedForDestruction() const { return m_IsDestroyed; }

		const Transform& GetTransform() const { return m_Transform; }
		Vec3 FindPosition() const { return m_Transform.FindPosition(); }
		Quat FindRotation() const { return m_Transform.FindRotation(); }
		Vec3 FindRotationEuler() const { return m_Transform.FindRotationEuler(); }
		Vec3 FindScale() const { return m_Transform.FindScale(); }
		const Vec3& GetLocalPosition() const { return m_Transform.GetLocalPosition(); }
		const Quat& GetLocalRotation() const { return m_Transform.GetLocalRotation(); }
		const Vec3& GetLocalRotationEuler() const { return m_Transform.GetLocalRotationEuler(); }
		const Vec3& GetLocalScale() const { return m_Transform.GetLocalScale(); }
		void SetPosition(const Vec3& position) { m_Transform.SetPosition(position); }
		void SetRotation(const Quat& rotation) { m_Transform.SetRotation(rotation); }
		void SeRotation(const Vec3& rotationEuler) { m_Transform.SetRotation(rotationEuler); }
		void SetScale(const Vec3& scale) { m_Transform.SetScale(scale); }
		void SetLocalPosition(const Vec3& position) { m_Transform.SetLocalPosition(position); }
		void SetLocalRotation(const Quat& rotation) { m_Transform.SetLocalRotation(rotation); }
		void SetLocalRotation(const Vec3& rotationEuler) { m_Transform.SetLocalRotation(rotationEuler); }
		void SetLocalScale(const Vec3& scale) { m_Transform.SetLocalScale(scale); }

		GameObject* GetParent() const { return m_pParent; }
		size_t GetChildrenCount() const { return m_pChildren.size(); }
		GameObject* GetChild(size_t index) const;
		void SetParent(GameObject* parent);
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);

		void AddComponent(Component* component);
		void RemoveComponent(Component* component);

	protected:
		void RecalculateTransform();

	private:
		uint64_t m_ID = 0;
		std::string m_Name = std::string();
		Transform m_Transform;

		Scene* m_pScene = nullptr;

		GameObject* m_pParent = nullptr;
		std::vector<GameObject*> m_pChildren;

		bool m_IsDestroyed = false;

		std::vector<Component*> m_Components;
	};
}
