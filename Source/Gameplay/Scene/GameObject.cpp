#include "GameObject.h"

#include "Gameplay/Scene/Scene.h"
#include "Components/Component.h"

Arg::GameObject::GameObject(uint64_t id, Scene* scene, std::string name)
	: m_ID(id),
	m_Name(std::move(name)),
	m_pScene(scene)
{

}

bool Arg::GameObject::operator==(const GameObject& other) const
{
	return this->m_ID == other.m_ID;
}

Arg::Scene* Arg::GameObject::GetScene() const
{
	return m_pScene;
}

void Arg::GameObject::Tick(const GameTime& gameTime) const
{
	if (m_IsDestroyed)
	{
		return;
	}

	for (Component* component : m_Components)
	{
		component->Tick(gameTime);
	}
}

void Arg::GameObject::Render(const RenderContext& renderContext) const
{
	if (m_IsDestroyed)
	{
		return;
	}

	for (Component* component : m_Components)
	{
		component->Render(renderContext);
	}
}

void Arg::GameObject::PrepareForRender()
{
	if (m_IsDestroyed)
	{
		return;
	}

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

void Arg::GameObject::Destroy()
{
	if (m_IsDestroyed)
	{
		return;
	}

	m_IsDestroyed = true;
}

Arg::GameObject* Arg::GameObject::GetChild(size_t index) const
{
	if (index < 0 || index >= m_pChildren.size())
	{
		return nullptr;
	}

	return m_pChildren[index];
}

void Arg::GameObject::SetParent(GameObject* parent)
{
	if (m_pParent != nullptr)
	{
		m_pParent->RemoveChild(this);
	}

	m_pParent = parent;
	m_Transform.ChangeParent(parent->GetTransform().GetGlobalTransform());
}

void Arg::GameObject::AddChild(GameObject* child)
{
	child->SetParent(this);
	m_pChildren.push_back(child);
	m_Transform.SetDirty();
}

void Arg::GameObject::RemoveChild(GameObject* child)
{
	const auto& it = std::ranges::find(m_pChildren, child);
	if (it != m_pChildren.end())
	{
		m_pChildren.erase(it);
	}
}

Arg::Component* Arg::GameObject::GetComponent(size_t index) const
{
	if (index < 0 || index >= m_Components.size())
	{
		return nullptr;
	}

	return m_Components[index];
}

void Arg::GameObject::AddComponent(Component* component)
{
	m_Components.push_back(component);
}

void Arg::GameObject::RemoveComponent(Component* component)
{
	const auto it = std::ranges::find(m_Components, component);
	if (it != m_Components.end())
	{
		m_Components.erase(it);
	}
}

void Arg::GameObject::RecalculateTransform()
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
