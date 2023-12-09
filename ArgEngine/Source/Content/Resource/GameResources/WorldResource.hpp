#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/GameResource.hpp"
#include "Gameplay/World/GameWorld.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string WORLD_RESOURCE_FILE_EXTENSION = ".amap";

		class WorldResource : public GameResource
		{
		public:
			WorldResource(const std::shared_ptr<Resource>& resource);
			WorldResource(const WorldResource&) = delete;
			~WorldResource() = default;

			auto GetWorld() const -> const std::shared_ptr<Gameplay::GameWorld>&
			{
				return m_pWorld;
			}

		protected:
			auto GetResourceFileExtension() const -> const std::string & override;

			void VPreLoad() override;
			auto VGetSerializableData() const->ISerializable* override;
			void VUnload() override;

		private:
			std::shared_ptr<Gameplay::GameWorld> m_pWorld = nullptr;
		};
	}
}
