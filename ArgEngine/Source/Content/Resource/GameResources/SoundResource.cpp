#include <arg_pch.hpp>
#include "SoundResource.hpp"

auto Arg::Content::SoundData::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["Sound"];
	header["Format"] = Format;

	return true;
}

auto Arg::Content::SoundData::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto header = node["Sound"];
	if (!header)
	{
		return false;
	}

	Format = ValueOr<std::string>(header["Format"], "");

	return true;
}

Arg::Content::SoundResource::SoundResource(const std::shared_ptr<Resource>& resource)
	: GameDataResource(resource)
{
}

auto Arg::Content::SoundResource::GetSoundData() -> char*
{
	GameDataResource::VPreLoad();
	return GetData()->data();
}

auto Arg::Content::SoundResource::GetSoundDataSize() const -> size_t
{
	return GetData()->size();
}

void Arg::Content::SoundResource::FreeSoundData()
{
	GameDataResource::VPostLoad();
}

auto Arg::Content::SoundResource::GetFormat() const -> const std::string&
{
	return m_pSoundData->Format;
}

auto Arg::Content::SoundResource::GetResourceFileExtension() const -> const std::string&
{
	return SOUND_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::SoundResource::VPreLoad()
{
	GameDataResource::VPreLoad();
	m_pSoundData = std::make_unique<SoundData>();
}

void Arg::Content::SoundResource::VOnLoad()
{
	GameDataResource::VOnLoad();
}

auto Arg::Content::SoundResource::VGetSerializableData() const -> ISerializable*
{
	return m_pSoundData.get();
}

void Arg::Content::SoundResource::VUnload()
{
	m_pSoundData = nullptr;
}
