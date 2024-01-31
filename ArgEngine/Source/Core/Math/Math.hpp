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

		inline auto ToEuler(const Quat& rotation) -> Vec3
		{
			const Vec3 eulerAngles = Math::eulerAngles(rotation);
			return {eulerAngles.x, eulerAngles.y, eulerAngles.z};
		}

		inline auto ToQuat(const Vec3& eulerAngles) -> Quat
		{
			const Quat rotation(Vec3(eulerAngles.x, eulerAngles.y, eulerAngles.z));
			return rotation;
		}

		constexpr Vec3 VEC3_ZERO = Vec3(0.0f);
		constexpr Vec3 VEC3_ONE = Vec3(1.0f);
		constexpr Quat QUAT_ZERO = Quat();
		constexpr Mat4 MAT4_IDENTITY = Mat4(1.0f);

		inline void Decompose(
			const Mat4& transform,
			Vec3& outPosition,
			Quat& outRotation,
			Vec3& outScale
		)
		{
			outPosition = Vec3(transform[3]);

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
			Vec3& outPosition,
			Vec3& outRotation,
			Vec3& outScale
		)
		{
			Quat rotation;
			Decompose(transform, outPosition, rotation, outScale);
			outRotation = Math::ToEuler(rotation);
		}

		inline auto CalculateTransform(
			const Vec3& position,
			const Quat& rotation,
			const Vec3& scale
		) -> Mat4
		{
			Mat4 transform = Math::MAT4_IDENTITY;
			transform = Math::translate(transform, position);
			transform = transform * Math::mat4_cast(rotation);
			transform = Math::scale(transform, scale);
			return transform;
		}

		inline auto CalculateTransform(
			const Vec3& position,
			const Vec3& rotationEuler,
			const Vec3& scale
		) -> Mat4
		{
			const Quat rotation = Math::ToQuat(rotationEuler);
			return CalculateTransform(position, rotation, scale);
		}

		inline auto ForwardVecFromRotation(float pitch, float yaw, float roll) -> Vec3
		{
			return Math::normalize(Vec3(
				Math::cos(yaw)
				* Math::cos(-pitch),

				Math::sin(yaw)
				* Math::cos(-pitch),

				Math::sin(-pitch)
			));
		}

		inline auto ForwardVecFromRotation(const Vec3& rotationEuler) -> Vec3
		{
			return Math::ForwardVecFromRotation(rotationEuler.y, rotationEuler.z, rotationEuler.x);
		}

		inline auto RightVecFromRotation(float pitch, float yaw, float roll) -> Vec3
		{
			return -Math::normalize(Vec3(
				-Math::cos(yaw)
				* Math::sin(-pitch)
				* Math::sin(roll)
				- Math::sin(yaw)
				* Math::cos(roll),

				-Math::sin(yaw)
				* Math::sin(-pitch)
				* Math::sin(roll)
				+ Math::cos(yaw)
				* Math::cos(roll),

				Math::cos(-pitch)
				* Math::sin(roll)
			));
		}

		inline auto RightVecFromRotation(const Vec3& rotationEuler) -> Vec3
		{
			return Math::RightVecFromRotation(rotationEuler.y, rotationEuler.z, rotationEuler.x);
		}

		inline auto UpVecFromRotation(float pitch, float yaw, float roll) -> Vec3
		{
			return Math::normalize(Vec3(
				-Math::cos(yaw)
				* Math::sin(-pitch)
				* Math::cos(roll)
				+ Math::sin(yaw)
				* Math::sin(roll),

				-Math::sin(yaw)
				* Math::sin(-pitch)
				* Math::cos(roll)
				- Math::cos(yaw)
				* Math::sin(roll),

				Math::cos(-pitch)
				* Math::cos(roll)
			));
		}

		inline auto UpVecFromRotation(const Vec3& rotationEuler) -> Vec3
		{
			return Math::UpVecFromRotation(rotationEuler.y, rotationEuler.z, rotationEuler.x);
		}
	}
}
