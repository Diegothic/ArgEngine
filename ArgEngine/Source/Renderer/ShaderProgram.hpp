#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct ShaderSource
		{
			std::string VertexSource;
			std::string FragmentSource;
		};

		class ShaderProgram
		{
		public:
			ShaderProgram();
			ShaderProgram(const ShaderProgram&) = delete;
			~ShaderProgram();

			auto Create(const ShaderSource& source) -> bool;
			void Use() const;

			void SetUniform(const std::string& name, int32_t value);
			void SetUniform(const std::string& name, float value);
			void SetUniform(const std::string& name, bool value);
			void SetUniform(const std::string& name, Vec2 value);
			void SetUniform(const std::string& name, Vec3 value);
			void SetUniform(const std::string& name, Vec4 value);
			void SetUniform(const std::string& name, const Mat3& value);
			void SetUniform(const std::string& name, const Mat4& value);

		private:
			static auto Compile(uint32_t shaderID, const std::string& source) -> bool;

		private:
			void Attach(uint32_t shaderID) const;
			auto Link() const -> bool;
			auto FindUniformLocation(const std::string& name) -> int;

		private:
			uint32_t m_RendererID = 0;
			std::unordered_map<std::string, int> m_UniformLocations;
		};
	}
}
