#include "Resource.h"

#include "Content.h"
#include "Core/File/FileReader.h"

Arg::Resource::Resource(Content* content, GUID ID, std::string name, const YAML::Node& dataNode)
	: m_Content(content),
	m_ID(ID),
	m_Name(std::move(name))
{
}

void Arg::Resource::Load()
{
	if (m_IsLoaded)
	{
		return;
	}

	FileReader reader(m_Content->GetResourcePath(m_ID));
	reader.ReadBinary();
	const char* data = reader.GetData();
	VOnLoad(data);

	m_IsLoaded = true;
}

void Arg::Resource::Unload()
{
	if (!m_IsLoaded)
	{
		return;
	}

	VOnUnload();

	m_IsLoaded = false;
}
