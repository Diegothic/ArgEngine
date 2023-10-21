#pragma once

#include <cstdint>
#include <Arg/Memory.h>

#include "Scene.h"
#include "Components/Transform.h"

namespace Arg
{
	class GameObject
	{
	public:
		GameObject(uint64_t id, const std::string& name = std::string())
			: m_ID(id),
			m_Name(name)
		{
		}
		~GameObject() = default;

		bool operator==(const GameObject& other) const
		{
			return this->m_ID == other.m_ID;
		}

		uint64_t GetID() const { return m_ID; }
		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name){ m_Name = name; }

		void Tick(const GameTime& gameTime)
		{

		}

		void Render(Box<Renderer>& renderer)
		{

		}

		void PrepareForRender()
		{
			if (m_Transform.IsDirty())
			{
				RecalculateTransform();
			}
			else
			{
				for (const auto& child : m_pChildren)
				{
					child->PrepareForRender();
				}
			}
		}

		const Transform& GetTransform() const { return m_Transform; }
		const Vec3& GetPosition() const { return m_Transform.GetPosition(); }
		const Quat& GetRotation() const { return m_Transform.GetRotation(); }
		Vec3 GetRotationEuler() const { return m_Transform.GetRotationEuler(); }
		const Vec3& GetScale() const { return m_Transform.GetScale(); }

		void SetPosition(const Vec3& position) { m_Transform.SetPosition(position); }
		void SetRotation(const Quat& rotation) { m_Transform.SetRotation(rotation); }
		void SetRotation(const Vec3& rotationEuler) { m_Transform.SetRotation(rotationEuler); }
		void SetScale(const Vec3& scale) { m_Transform.SetScale(scale); }

		GameObject* GetParent() const { return m_pParent; }
		size_t GetChildrenCount() const { return m_pChildren.size(); }
		GameObject* GetChild(size_t index) const
		{
			if (index < 0 || index >= m_pChildren.size())
			{
				return nullptr;
			}

			return m_pChildren[index];
		}
		void SetParent(GameObject* parent)
		{
			if (m_pParent != nullptr)
			{
				m_pParent->RemoveChild(this);
			}

			m_pParent = parent;
			m_Transform.ChangeParent(parent->GetTransform().GetGlobalTransform());
		}
		void AddChild(GameObject* child)
		{
			child->SetParent(this);
			m_pChildren.push_back(child);
			m_Transform.SetDirty();
		}
		void RemoveChild(GameObject* child)
		{
			const auto& it = std::ranges::find(m_pChildren, child);
			if (it != m_pChildren.end())
			{
				m_pChildren.erase(it);
			}
		}

	protected:
		void RecalculateTransform()
		{
			if (m_pParent != nullptr)
			{
				m_Transform.Recalculate(m_pParent->GetTransform().GetGlobalTransform());
			}

			for (const auto& child : m_pChildren)
			{
				child->RecalculateTransform();
			}
		}

	private:
		uint64_t m_ID;
		std::string m_Name;
		Transform m_Transform;

		GameObject* m_pParent = nullptr;
		std::vector<GameObject*> m_pChildren;
	};
}
