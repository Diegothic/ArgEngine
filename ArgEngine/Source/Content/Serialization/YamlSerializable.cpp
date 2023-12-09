#include <arg_pch.hpp>
#include "YamlSerializable.hpp"

auto Arg::Content::YamlSerializable::Serialize(
	const std::filesystem::path& destination
) const -> bool
{
	YAML::Node data;
	if (std::filesystem::exists(destination))
	{
		data = YAML::LoadFile(destination.string());
	}

	const bool bIsSuccess = VOnSerialize(data);
	if (!bIsSuccess)
	{
		return false;
	}

	std::ofstream file(
		destination,
		std::ofstream::out
		| std::ofstream::trunc
	);
	file << data;
	file.close();

	return true;
}

auto Arg::Content::YamlSerializable::Deserialize(
	const std::filesystem::path& source
) -> bool
{
	if (!std::filesystem::exists(source))
	{
		return false;
	}

	YAML::Node data = YAML::LoadFile(source.string());

	const bool bIsSuccess = VOnDeserialize(data);
	if (!bIsSuccess)
	{
		return false;
	}

	return true;
}

auto Arg::Content::YamlSerializable::Serialize(YAML::Node& node) const -> bool
{
	const bool bIsSuccess = VOnSerialize(node);
	return bIsSuccess;
}

auto Arg::Content::YamlSerializable::Deserialize(const YAML::Node& node) -> bool
{
	const bool bIsSuccess = VOnDeserialize(node);
	return bIsSuccess;
}
