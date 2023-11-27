#include <arg_pch.hpp>
#include "ShaderProgram.hpp"

#include <glad/glad.h>

#include "Debug/Assert.hpp"

Arg::Renderer::ShaderProgram::ShaderProgram()
{
	m_RendererID = glCreateProgram();
}

Arg::Renderer::ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_RendererID);
}

auto Arg::Renderer::ShaderProgram::Create(const ShaderSource& source) -> bool
{
	bool bIsSuccess = false;
	const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	bIsSuccess = ShaderProgram::Compile(vertexShader, source.VertexSource);
	if (!bIsSuccess)
	{
		return false;
	}

	const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	bIsSuccess = ShaderProgram::Compile(fragmentShader, source.FragmentSource);
	if (!bIsSuccess)
	{
		return false;
	}

	Attach(vertexShader);
	Attach(fragmentShader);
	bIsSuccess = Link();
	if (!bIsSuccess)
	{
		return false;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void Arg::Renderer::ShaderProgram::Use() const
{
	glUseProgram(m_RendererID);
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, int32_t value)
{
	glUniform1i(FindUniformLocation(name), value);
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, float value)
{
	glUniform1f(FindUniformLocation(name), value);
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, bool value)
{
	SetUniform(name, value ? 1 : 0);
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, Vec2 value)
{
	glUniform2f(FindUniformLocation(name), value.x, value.y);
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, Vec3 value)
{
	glUniform3f(FindUniformLocation(name), value.x, value.y, value.z);
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, Vec4 value)
{
	glUniform4f(FindUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, const Mat3& value)
{
	glUniformMatrix3fv(FindUniformLocation(name), 1, GL_FALSE, Math::ValuePtr(value));
}

void Arg::Renderer::ShaderProgram::SetUniform(const std::string& name, const Mat4& value)
{
	glUniformMatrix4fv(FindUniformLocation(name), 1, GL_FALSE, Math::ValuePtr(value));
}

auto Arg::Renderer::ShaderProgram::Compile(uint32_t shaderID, const std::string& source) -> bool
{
	const char* shaderSource = source.c_str();
	glShaderSource(shaderID, 1, &shaderSource, nullptr);
	glCompileShader(shaderID);

	int bIsSuccess = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &bIsSuccess);
	if (!bIsSuccess)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
		ARG_CONSOLE_LOG_ERR("Shader compilation failed!\n Source: %s\n Reason: %s", source.c_str(), infoLog);
		return false;
	}

	return true;
}

void Arg::Renderer::ShaderProgram::Attach(uint32_t shaderID) const
{
	glAttachShader(m_RendererID, shaderID);
}

auto Arg::Renderer::ShaderProgram::Link() const -> bool
{
	glLinkProgram(m_RendererID);

	int bIsSuccess = 0;
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &bIsSuccess);
	if (!bIsSuccess)
	{
		char infoLog[512];
		glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
		ARG_CONSOLE_LOG_ERR("Shader Program link failed!\n Reason: %s", infoLog);
		return false;
	}

	return true;
}

auto Arg::Renderer::ShaderProgram::FindUniformLocation(const std::string& name) -> int
{
	if (m_UniformLocations.contains(name))
	{
		return m_UniformLocations.at(name);
	}

	const auto uniformLocation = glGetUniformLocation(m_RendererID, name.c_str());
	ARG_ASSERT(uniformLocation != -1, "Uniform not found!");
	m_UniformLocations[name] = uniformLocation;
	return uniformLocation;
}
