#pragma once

#include <arg_pch.hpp>

#include "Material.hpp"

namespace Arg
{
	namespace Renderer
	{
		class CubeMap
		{
		public:
			CubeMap();
			CubeMap(const CubeMap&) = delete;
			~CubeMap();

			auto GetRendererID() const -> const uint32_t& { return m_RendererID; }

			void SetFaces(const std::vector<TextureHandle>& textures) const;

			void Bind() const;
			void Bind(int32_t unit) const;
			void Unbind() const;

		private:
			uint32_t m_RendererID = -1;
		};
	}
}
