#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/GameResource.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string RESOURCE_DATA_FILE_EXTENSION = ".data";

		class GameDataResource : public GameResource
		{
		public:
			GameDataResource(const std::shared_ptr<Resource>& resource);
			GameDataResource(const GameDataResource&) = delete;

		public:
			virtual auto VIsSaveable() const -> bool override { return false; }
			virtual void VRemoveFiles() override;
			virtual void VRenameFiles(const std::string& name) override;
			virtual void VMoveFiles(const std::filesystem::path& destination) override;

		protected:
			virtual void VPreLoad() override;
			virtual void VPostLoad() override;

			auto GetData() const -> const std::unique_ptr<std::vector<char>>& { return m_pData; }
			auto GetDataFilePath() const->std::filesystem::path;

		private:
			std::unique_ptr<std::vector<char>> m_pData = nullptr;
		};
	}
}