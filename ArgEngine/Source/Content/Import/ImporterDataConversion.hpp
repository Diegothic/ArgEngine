#pragma once

#include <arg_pch.hpp>

#include <assimp/Importer.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Import
	{
		namespace DataConversion
		{
			auto Convert(const aiString& name) -> std::string;
			auto Convert(const aiVector2D& vec) -> Vec2;
			auto Convert(const aiVector3D& vec) -> Vec3;
			auto Convert(const aiQuaternion& quat) -> Quat;
			auto Convert(const aiMatrix3x3& mat) -> Mat3;
			auto Convert(const aiMatrix4x4& mat) -> Mat4;
		}
	}
}
