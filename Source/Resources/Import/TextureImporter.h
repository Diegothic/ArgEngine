#pragma once

#include <filesystem>
#include <stb/stb_image.h>
#include <yaml-cpp/yaml.h>

#include "Core/GUID.h"
#include "Core/File/FileWriter.h"

namespace Arg
{
	class TextureImporter
	{
	public:
		TextureImporter(std::filesystem::path path) : m_Path(std::move(path)) {}

		void Import(const std::string& name,
			const std::filesystem::path& rootDirectory,
			const std::filesystem::path& destination
		) const
		{
			stbi_set_flip_vertically_on_load(true);
			int width;
			int height;
			int nrChannels;
			unsigned char* textureData = stbi_load(
				m_Path.string().c_str(),
				&width,
				&height,
				&nrChannels,
				0);

			std::filesystem::path filePath = destination / name;

			std::filesystem::path dataPath = filePath.replace_extension(".tex");
			{
				const FileWriter dataWriter(dataPath);
				dataWriter.WriteBinary((char*)textureData, width * height * nrChannels);
			}

			stbi_image_free(textureData);

			std::filesystem::path resourcePath = filePath.replace_extension(".rsrc");
			std::filesystem::path relative = std::filesystem::relative(destination, rootDirectory) / name;
			uint64_t ID = std::hash<std::string>{}(relative.string());

			YAML::Node node;
			node["resource"]["id"] = ID;
			node["resource"]["type"] = "Texture";
			node["resource"]["file"] = dataPath.filename().string();

			node["data"]["width"] = width;
			node["data"]["height"] = height;
			node["data"]["format"] = nrChannels;

			{
				const FileWriter resourceWriter(resourcePath);
				std::stringstream outStream;
				outStream << node;
				resourceWriter.Write(outStream.str());
			}
		}

	private:
		std::filesystem::path m_Path;
	};
}
