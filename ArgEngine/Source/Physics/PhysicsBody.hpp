#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"
#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Physics
	{
		enum PhysicsBodyShape
		{
			Box,
			Sphere,
			Capsule
		};
		
		struct PhysicsBody
		{
			GUID ID = GUID::Empty;

			PhysicsBodyShape Shape = PhysicsBodyShape::Box;
			Vec3 Size = Vec3(1.0f, 1.0f, 1.0f);
			
			bool bIsDynamic = true;
			float Mass = 1.0f;
			float Damping = 0.05f;
			float AngularDamping = 0.05f;
			float Bounciness = 0.0f;
			
			bool LockMovement[3] = {false, false, false};
			bool LockRotation[3] = {false, false, false};
		};
	}
}
