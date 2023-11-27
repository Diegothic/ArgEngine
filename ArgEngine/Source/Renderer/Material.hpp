#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Renderer
	{
		class Material
		{
		public:
			Material() {};
			virtual ~Material() {};

			void Apply() {};
		};
	}
}
