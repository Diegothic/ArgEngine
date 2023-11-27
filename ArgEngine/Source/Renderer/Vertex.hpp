#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Renderer
	{
		struct StaticVertex
		{
			Vec3 Position;
			Vec3 Normal;
			Vec2 TexUV;
		};

		struct SkeletalVertex
		{
			Vec3 Position;
			Vec3 Normal;
			Vec2 TexUV;

			Vec4i BoneIndices;
			Vec4 BoneWeights;
		};
	}
}
