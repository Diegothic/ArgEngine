#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Arg
{
	namespace Math
	{
		using namespace glm;

		template<typename T>
		constexpr typename T::value_type const* Ref(T const& v)
		{
			return glm::value_ptr(v);
		}

		template<typename T>
		constexpr typename T::value_type* Ref(T& v)
		{
			return glm::value_ptr(v);
		}
	}

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
}
