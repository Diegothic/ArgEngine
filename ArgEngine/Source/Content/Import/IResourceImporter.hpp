#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Import
	{
		class IResourceImporter
		{
		public:
			virtual auto ImportFile(const std::string& file) -> bool = 0;
			virtual void Save(
				const std::string& fileName,
				const std::filesystem::path& destination
			) = 0;
		};
	}
}
