#include "StaticMeshComponent.h"

#include "Gameplay/Scene/GameObject.h"

Arg::StaticMeshComponent::StaticMeshComponent(uint64_t ID, GameObject* owner)
	: Component(ID, owner)
{
	const float cubeVertices[] = {
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
	};

	const uint32_t cubeIndices[] = {
		0,1,2,
		0,2,3,
		1,5,6,
		1,6,2,
		5,4,7,
		5,7,6,
		4,0,3,
		4,3,7,
		3,2,6,
		3,6,7,
		0,1,5,
		0,5,4
	};

	uint32_t VBO;
	glGenBuffers(1, &VBO);

	uint32_t EBO;
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &m_VertexArrayHandle);
	glBindVertexArray(m_VertexArrayHandle);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Arg::StaticMeshComponent::Render(const RenderContext& renderContext)
{
	const Mat4 transform = GetOwner()->GetTransform().GetGlobalTransform();

	glUniformMatrix4fv(glGetUniformLocation(renderContext.Shader, "u_Model"), 1, GL_FALSE, Math::Ref(transform));
	glUniform3f(glGetUniformLocation(renderContext.Shader, "u_Color"), 1.0f, 1.0f, 1.0f);

	glBindVertexArray(m_VertexArrayHandle);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
