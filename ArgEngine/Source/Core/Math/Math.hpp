#pragma once

#include <arg_pch.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace Arg
{
	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Vec4 = glm::vec4;

	using Vec2i = glm::ivec2;
	using Vec3i = glm::ivec3;
	using Vec4i = glm::ivec4;

	using Vec2u = glm::uvec2;
	using Vec3u = glm::uvec3;
	using Vec4u = glm::uvec4;

	using Quat = glm::quat;

	using Mat3 = glm::mat3;
	using Mat4 = glm::mat4;

	namespace Math
	{
		using namespace glm;

		template <typename T>
		constexpr auto ValuePtr(T const& v) -> typename T::value_type const*
		{
			return glm::value_ptr(v);
		}

		template <typename T>
		constexpr auto ValuePtr(T& v) -> typename T::value_type*
		{
			return glm::value_ptr(v);
		}

		inline void Decompose(
			const Mat4& transform,
			Vec3& outTranslation,
			Quat& outRotation,
			Vec3& outScale
		)
		{
			outTranslation = Vec3(transform[3]);

			outScale.x = Math::length(Vec3(transform[0]));
			outScale.y = Math::length(Vec3(transform[1]));
			outScale.z = Math::length(Vec3(transform[2]));

			Mat4 rotationMatrix = transform;
			rotationMatrix[0] = Vec4(Math::normalize(Vec3(rotationMatrix[0])), 0.0f);
			rotationMatrix[1] = Vec4(Math::normalize(Vec3(rotationMatrix[1])), 0.0f);
			rotationMatrix[2] = Vec4(Math::normalize(Vec3(rotationMatrix[2])), 0.0f);
			outRotation = Math::quat_cast(rotationMatrix);
		}

		inline void Decompose(
			const Mat4& transform,
			Vec3& outTranslation,
			Vec3& outRotation,
			Vec3& outScale
		)
		{
			outTranslation = Vec3(transform[3]);

			outScale.x = Math::length(Vec3(transform[0]));
			outScale.y = Math::length(Vec3(transform[1]));
			outScale.z = Math::length(Vec3(transform[2]));

			Mat4 rotationMatrix = transform;
			rotationMatrix[0] = Vec4(Math::normalize(Vec3(rotationMatrix[0])), 0.0f);
			rotationMatrix[1] = Vec4(Math::normalize(Vec3(rotationMatrix[1])), 0.0f);
			rotationMatrix[2] = Vec4(Math::normalize(Vec3(rotationMatrix[2])), 0.0f);
			const Quat rotationQuaternion = Math::quat_cast(rotationMatrix);
			outRotation = Math::eulerAngles(rotationQuaternion);
		}

		inline auto ForwardVecFromRotation(float pitch, float yaw, float roll) -> Vec3
		{
			return Math::normalize(Vec3(
				Math::cos(yaw)
				* Math::cos(pitch),

				Math::sin(yaw)
				* Math::cos(pitch),

				Math::sin(pitch)
			));
		}

		inline auto RightVecFromRotation(float pitch, float yaw, float roll) -> Vec3
		{
			return -Math::normalize(Vec3(
				-Math::cos(yaw)
				* Math::sin(pitch)
				* Math::sin(roll)
				- Math::sin(yaw)
				* Math::cos(roll),

				-Math::sin(yaw)
				* Math::sin(pitch)
				* Math::sin(roll)
				+ Math::cos(yaw)
				* Math::cos(roll),

				Math::cos(pitch)
				* Math::sin(roll)
			));
		}

		inline auto UpVecFromRotation(float pitch, float yaw, float roll) -> Vec3
		{
			return Math::normalize(Vec3(
				-Math::cos(yaw)
				* Math::sin(pitch)
				* Math::cos(roll)
				+ Math::sin(yaw)
				* Math::sin(roll),

				-Math::sin(yaw)
				* Math::sin(pitch)
				* Math::cos(roll)
				- Math::cos(yaw)
				* Math::sin(roll),

				Math::cos(pitch)
				* Math::cos(roll)
			));
		}
	}
}
