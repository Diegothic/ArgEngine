#include "Scene.h"

#include <iostream>
#include <glad/glad.h>

#include "Renderer/RenderContext.h"
#include "Component/Component.h"
#include "Resources/ResourceTypes/TextureResource.h"

void Arg::Scene::Start()
{
	m_GameTime.ElapsedTime = 0.0f;
	m_GameTime.DeltaTime = 1.0f / 30.0f;

	m_RootObject = NewBox<GameObject>(1, this, "SceneRoot");

	// TEMP:
	vertexSource = R"(
				#version 460 core
				layout (location = 0) in vec3 l_Position;

				out vec3 o_Color;

				uniform vec3 u_Color;
				uniform mat4 u_Model;
				uniform mat4 u_View;
				uniform mat4 u_Projection;

				void main()
				{
					gl_Position = u_Projection * u_View * u_Model * vec4(l_Position, 1.0);
					o_Color = u_Color;
				}
				)";

	fragmentSource = R"(
				#version 460 core
				out vec4 FragColor;

				in vec3 o_Color;

				void main()
				{
					float z = gl_FragCoord.z * 2.0 - 1.0;
					float depth = ((2.0 * 0.1 * 5.0) / (5.0 + 0.1 - z * (5.0 - 0.1))) / 5.0;
					
					FragColor = vec4(o_Color * (1.0 - depth), 1.0);
				} 
				)";

	vertexSource2 = R"(
				#version 460 core
				layout (location = 0) in vec3 l_Position;
				layout (location = 1) in vec3 l_Normal;
				layout (location = 2) in vec2 l_TexCoords;
				
				uniform mat4 u_Model;
				uniform mat4 u_View;
				uniform mat4 u_Projection;

				out vec2 v_TexCoords;

				void main()
				{
					gl_Position = u_Projection * u_View * u_Model * vec4(l_Position, 1.0);
					v_TexCoords = l_TexCoords;
				}
				)";

	fragmentSource2 = R"(
				#version 460 core
				out vec4 FragColor;

				in vec2 v_TexCoords;

				uniform sampler2D u_Texture0;
				uniform vec3 u_Color;

				void main()
				{
					float z = gl_FragCoord.z * 2.0 - 1.0;
					float depth = ((2.0 * 0.1 * 5.0) / (5.0 + 0.1 - z * (5.0 - 0.1))) / 5.0;
					
					vec3 diffuse = vec3(texture(u_Texture0, v_TexCoords)) * u_Color;
					FragColor = vec4(diffuse * (1.0 - depth), 1.0);
				} 
				)";

	const char* vertexSourceBuffer = vertexSource.c_str();
	const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSourceBuffer, nullptr);
	glCompileShader(vertexShader);

	const char* fragmentSourceBuffer = fragmentSource.c_str();
	const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSourceBuffer, nullptr);
	glCompileShader(fragmentShader);

	shader = glCreateProgram();
	glUseProgram(shader);
	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(0);

	const char* vertexSourceBuffer2 = vertexSource2.c_str();
	const uint32_t vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vertexSourceBuffer2, nullptr);
	glCompileShader(vertexShader2);

	const char* fragmentSourceBuffer2 = fragmentSource2.c_str();
	const uint32_t fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fragmentSourceBuffer2, nullptr);
	glCompileShader(fragmentShader2);

	shader2 = glCreateProgram();
	glUseProgram(shader2);
	glAttachShader(shader2, vertexShader2);
	glAttachShader(shader2, fragmentShader2);
	glLinkProgram(shader2);

	int success;
	char infoLog[512];
	glGetProgramiv(shader2, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader2, 512, nullptr, infoLog);
		std::cout << "Shader Program link failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader2);
	glDeleteShader(fragmentShader2);

	glUseProgram(0);

	const float quadVertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
	};

	const uint32_t quadIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	uint32_t VBO;
	glGenBuffers(1, &VBO);

	uint32_t EBO;
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &quadVertexArray);
	glBindVertexArray(quadVertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	content.Initialize();
}

void Arg::Scene::Tick(double deltaTime)
{
	m_GameTime.DeltaTime = static_cast<float>(deltaTime);
	m_GameTime.ElapsedTime += m_GameTime.DeltaTime;

	for (const auto& gameObject : m_GameObjects)
	{
		gameObject->Tick(m_GameTime);
	}
}

void Arg::Scene::Render(Renderer* renderer)
{
	RenderContext renderContext{
		.Renderer = renderer,
		.Shader = shader,
		.Content = &content,
	};

	const Mat4 view = Math::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
	const Vec2i frameSize = renderer->GetFrameSize();
	const float aspectRatio = (float)frameSize.x / (float)frameSize.y;

	const Mat4 projection = Math::perspectiveLH(
		Math::radians(45.0f),
		aspectRatio,
		0.1f,
		1000.0f
	);

	renderContext.Shader = shader2;
	glUseProgram(shader2);

	glUniformMatrix4fv(glGetUniformLocation(shader2, "u_View"), 1, GL_FALSE, Math::Ref(view));
	glUniformMatrix4fv(glGetUniformLocation(shader2, "u_Projection"), 1, GL_FALSE, Math::Ref(projection));

	m_RootObject->PrepareForRender();
	for (const Rc<GameObject>& gameObject : m_GameObjects)
	{
		gameObject->Render(renderContext);
	}

	// Gizmos
	renderContext.Shader = shader;
	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_View"), 1, GL_FALSE, Math::Ref(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_Projection"), 1, GL_FALSE, Math::Ref(projection));

	const GameObject* selectedGameObject = FindGameObject(m_SelectedGameObjectID);
	if (selectedGameObject != nullptr)
	{
		const Vec3 position = selectedGameObject->FindPosition();
		const Quat rotation = selectedGameObject->FindRotation();

		glBindVertexArray(quadVertexArray);

		glClear(GL_DEPTH_BUFFER_BIT);

		{
			Mat4 model(1.0f);
			model = Math::translate(model, position);
			model *= Math::mat4_cast(rotation);
			model = Math::scale(model, Vec3(0.2f));
			model = Math::translate(model, Vec3(0.5f, 0.5f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, Math::Ref(model));
			glUniform3f(glGetUniformLocation(shader, "u_Color"), 1.0f, 0.0f, 0.0f);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}

		{
			Mat4 model(1.0f);
			model = Math::translate(model, position);
			model *= Math::mat4_cast(rotation);
			model = Math::rotate(model, Math::radians(90.0f), Vec3(0.0f, 1.0f, 0.0f));
			model = Math::scale(model, Vec3(0.2f));
			model = Math::translate(model, Vec3(-0.5f, 0.5f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, Math::Ref(model));
			glUniform3f(glGetUniformLocation(shader, "u_Color"), 0.0f, 1.0f, 0.0f);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}

		{
			Mat4 model(1.0f);
			model = Math::translate(model, position);
			model *= Math::mat4_cast(rotation);
			model = Math::rotate(model, Math::radians(-90.0f), Vec3(1.0f, 0.0f, 0.0f));
			model = Math::scale(model, Vec3(0.2f));
			model = Math::translate(model, Vec3(0.5f, -0.5f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, Math::Ref(model));
			glUniform3f(glGetUniformLocation(shader, "u_Color"), 0.0f, 0.0f, 1.0f);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}

		glBindVertexArray(0);
	}
}

void Arg::Scene::ClearGarbage()
{
	std::vector<uint64_t> gameObjectsToRemove;
	for (const Rc<GameObject>& gameObject : m_GameObjects)
	{
		if (gameObject->IsMarkedForDestruction())
		{
			gameObjectsToRemove.push_back(gameObject->GetID());
		}
	}

	for (const uint64_t& gameObjectID : gameObjectsToRemove)
	{
		RemoveGameObject(gameObjectID);
	}

	std::vector<uint64_t> componentsToRemove;
	for (const Rc<Component>& component : m_Components)
	{
		if (component->IsMarkedForDestruction())
		{
			componentsToRemove.push_back(component->GetID());
		}
	}

	for (const uint64_t& componentID : componentsToRemove)
	{
		RemoveComponent(componentID);
	}
}

Arg::GameObject* Arg::Scene::GetRootObject() const
{
	return m_RootObject.get();
}

Arg::GameObject* Arg::Scene::FindGameObject(uint64_t ID)
{
	if (!m_GameObjectsRegistry.contains(ID))
	{
		return nullptr;
	}

	return m_GameObjectsRegistry[ID];
}

uint64_t Arg::Scene::CreateGameObject(const std::string& name, uint64_t parentID)
{
	m_LastUsedID++;
	const Rc<GameObject> newObject = NewRc<GameObject>(m_LastUsedID, this, name);
	m_GameObjects.push_back(newObject);
	m_GameObjectsRegistry[m_LastUsedID] = newObject.get();
	GameObject* parent = parentID == 0 ? m_RootObject.get() : FindGameObject(parentID);
	parent->AddChild(newObject.get());
	return m_LastUsedID;
}

void Arg::Scene::DestroyGameObject(uint64_t ID)
{
	GameObject* gameObject = FindGameObject(ID);
	gameObject->Destroy();
}

void Arg::Scene::ChangeParentGameObject(uint64_t ID, uint64_t parentID)
{
	GameObject* gameObject = FindGameObject(ID);
	GameObject* newParent = parentID == 0 ? m_RootObject.get() : FindGameObject(parentID);
	newParent->AddChild(gameObject);
}

void Arg::Scene::RemoveGameObject(uint64_t ID)
{
	GameObject* gameObject = FindGameObject(ID);
	GameObject* parent = gameObject->GetParent();
	const size_t childrenCount = gameObject->GetChildrenCount();
	if (childrenCount > 0)
	{
		std::vector<GameObject*> children;
		for (size_t i = 0; i < childrenCount; i++)
		{
			children.push_back(gameObject->GetChild(i));
		}

		for (const auto& child : children)
		{
			parent->AddChild(child);
		}
	}
	parent->RemoveChild(gameObject);

	m_GameObjectsRegistry.erase(ID);
	const auto it = std::ranges::find_if(
		m_GameObjects,
		[&gameObject](const Rc<GameObject>& element)
		{
			return (*element.get()) == (*gameObject);
		}
	);
	if (it != m_GameObjects.end())
	{
		m_GameObjects.erase(it);
	}
}

Arg::Component* Arg::Scene::FindComponent(uint64_t ID)
{
	if (!m_ComponentsRegistry.contains(ID))
	{
		return nullptr;
	}

	return m_ComponentsRegistry[ID];
}

void Arg::Scene::DestroyComponent(uint64_t ID)
{
	Component* component = FindComponent(ID);
	component->Destroy();
}

void Arg::Scene::RemoveComponent(uint64_t ID)
{
	Component* component = FindComponent(ID);
	GameObject* owner = component->GetOwner();
	owner->RemoveComponent(component);

	m_ComponentsRegistry.erase(ID);
	const auto it = std::ranges::find_if(
		m_Components,
		[&component](const Rc<Component>& element)
		{
			return (*element.get()) == (*component);
		}
	);
	if (it != m_Components.end())
	{
		m_Components.erase(it);
	}
}
