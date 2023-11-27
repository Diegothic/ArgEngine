#include <arg_pch.hpp>
#include "ImporterDataConversion.hpp"

auto Arg::Import::DataConversion::Convert(const aiString& name) -> std::string
{
	return std::string(name.C_Str());
}

auto Arg::Import::DataConversion::Convert(const aiVector2D& vec) -> Vec2
{
	return Vec2(vec.x, vec.y);
}

auto Arg::Import::DataConversion::Convert(const aiVector3D& vec) -> Vec3
{
	return Vec3(vec.x, vec.y, vec.z);
}

auto Arg::Import::DataConversion::Convert(const aiQuaternion& quat) -> Quat
{
	return Quat(quat.w, quat.x, quat.y, quat.z);
}

auto Arg::Import::DataConversion::Convert(const aiMatrix3x3& mat) -> Mat3
{
	Mat3 result;
	result[0][0] = mat.a1; result[1][0] = mat.a2; result[2][0] = mat.a3;
	result[0][1] = mat.b1; result[1][1] = mat.b2; result[2][1] = mat.b3;
	result[0][2] = mat.c1; result[1][2] = mat.c2; result[2][2] = mat.c3;
	return result;
}

auto Arg::Import::DataConversion::Convert(const aiMatrix4x4& mat) -> Mat4
{
	Mat4 result;
	result[0][0] = mat.a1; result[1][0] = mat.a2;
	result[2][0] = mat.a3; result[3][0] = mat.a4;
	result[0][1] = mat.b1; result[1][1] = mat.b2;
	result[2][1] = mat.b3; result[3][1] = mat.b4;
	result[0][2] = mat.c1; result[1][2] = mat.c2;
	result[2][2] = mat.c3; result[3][2] = mat.c4;
	result[0][3] = mat.d1; result[1][3] = mat.d2;
	result[2][3] = mat.d3; result[3][3] = mat.d4;
	return result;
}
