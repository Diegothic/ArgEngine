#include "Scene.h"

#include <glad/glad.h>

#include "GameObject.h"

void Arg::Scene::Start()
{
	m_GameTime.ElapsedTime = 0.0f;
	m_GameTime.DeltaTime = 1.0f / 30.0f;

	m_RootObject = NewBox<GameObject>(1, "SceneRoot");

	CreateGameObject("GameObject1");
	CreateGameObject("GameObject2", 2);
	CreateGameObject("GameObject3", 2);
	CreateGameObject("GameObject4", 3);
	CreateGameObject("GameObject5");

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
					FragColor = vec4(o_Color, 1.0);
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
	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

void Arg::Scene::Render(Box<Renderer>& renderer)
{
	m_RootObject->PrepareForRender();
	for (const auto& gameObject : m_GameObjects)
	{
		gameObject->Render(renderer);
	}

	glUseProgram(shader);

	Mat4 view = Math::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
	const float aspectRatio = (float)1920 / (float)1080;

	Mat4 projection = Math::perspective(
		Math::radians(45.0f),
		aspectRatio,
		0.1f,
		1000.0f
	);

	glUniformMatrix4fv(glGetUniformLocation(shader, "u_View"), 1, GL_FALSE, Math::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "u_Projection"), 1, GL_FALSE, Math::value_ptr(projection));

	for (const auto& gameObject : m_GameObjects)
	{
		// Draw object
		Mat4 model = gameObject->GetTransform().GetGlobalTransform();

		glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, Math::value_ptr(model));
		glUniform3f(glGetUniformLocation(shader, "u_Color"), 1.0f, 1.0f, 1.0f);

		glBindVertexArray(quadVertexArray);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	// Gizmos
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
			glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, Math::value_ptr(model));
			glUniform3f(glGetUniformLocation(shader, "u_Color"), 1.0f, 0.0f, 0.0f);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}

		{
			Mat4 model(1.0f);
			model = Math::translate(model, position);
			model *= Math::mat4_cast(rotation);
			model = Math::rotate(model, Math::radians(90.0f), Vec3(0.0f, 1.0f, 0.0f));
			model = Math::scale(model, Vec3(0.2f));
			model = Math::translate(model, Vec3(0.5f, 0.5f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, Math::value_ptr(model));
			glUniform3f(glGetUniformLocation(shader, "u_Color"), 0.0f, 1.0f, 0.0f);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}

		{
			Mat4 model(1.0f);
			model = Math::translate(model, position);
			model *= Math::mat4_cast(rotation);
			model = Math::rotate(model, Math::radians(90.0f), Vec3(1.0f, 0.0f, 0.0f));
			model = Math::scale(model, Vec3(0.2f));
			model = Math::translate(model, Vec3(0.5f, -0.5f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader, "u_Model"), 1, GL_FALSE, Math::value_ptr(model));
			glUniform3f(glGetUniformLocation(shader, "u_Color"), 0.0f, 0.0f, 1.0f);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}

		glBindVertexArray(0);
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
	const Rc<GameObject> newObject = NewRc<GameObject>(m_LastUsedID, name);
	m_GameObjects.push_back(newObject);
	m_GameObjectsRegistry[m_LastUsedID] = newObject.get();
	GameObject* parent = parentID == 0 ? m_RootObject.get() : FindGameObject(parentID);
	parent->AddChild(newObject.get());
	return m_LastUsedID;
}

void Arg::Scene::DestroyGameObject(uint64_t ID)
{
	//TODO: Mark for destruction -> Destroy on destruction cycle
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

void Arg::Scene::ChangeParentGameObject(uint64_t ID, uint64_t parentID)
{
	GameObject* gameObject = FindGameObject(ID);
	GameObject* newParent = parentID == 0 ? m_RootObject.get() : FindGameObject(parentID);
	newParent->AddChild(gameObject);
}
