#pragma once

#include "Core/Memory/SmartPtr.h"
#include "Renderer/Texture/Texture.h"
#include "Resources/Resource.h"

namespace Arg
{
	class TextureResource : public Resource
	{
	public:
		TextureResource(Content* content, GUID ID, std::string name, const YAML::Node& dataNode);

		Texture* GetTexture() const { return m_Texture.get(); }

	protected:
		void VOnLoad(const char* data) override;
		void VOnUnload() override;

	private:
		TextureSpec m_Spec;
		Box<Texture> m_Texture = nullptr;
	};
}
