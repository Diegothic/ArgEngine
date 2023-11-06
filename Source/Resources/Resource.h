#pragma once

#include <yaml-cpp/yaml.h>

#include "Core/GUID.h"

namespace Arg
{
	class Content;

	class Resource
	{
	public:
		Resource(Content* content, GUID ID, std::string name, const YAML::Node& dataNode);
		virtual ~Resource() = default;

		const GUID& GetID() const { return m_ID; }
		const std::string& GetName() const { return m_Name; }

		void Load();
		void Unload();
		bool IsLoaded() const { return m_IsLoaded; }

	protected:
		Content* GetContent() const { return m_Content; }

		virtual void VOnLoad(const char* data) = 0;
		virtual void VOnUnload() = 0;

	private:
		Content* m_Content = nullptr;

		GUID m_ID = 0;
		bool m_IsLoaded = false;
		std::string m_Name;
	};
}
