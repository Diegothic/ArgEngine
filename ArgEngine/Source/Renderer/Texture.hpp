#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Renderer
	{
		enum TextureFormat
		{
			FormatRGB,
			FormatRGBA,
			FormatDepth,
			FormatDepthStencil,
		};

		enum TextureFilter
		{
			FilterNearest,
			FilterLinear,
		};

		enum TextureWrapMode
		{
			WrapModeRepeat,
			WrapModeMirror,
			WrapModeClampToEdge,
			WrapModeClampToBorder,
		};

		enum TextureAttachmentSlot
		{
			AttachmentSlotColor,
			AttachmentSlotDepth,
			AttachmentSlotDepthStencil,
		};

		struct TextureData
		{
			void* Data = nullptr;
			int32_t Width = 0;
			int32_t Height = 0;
			TextureFormat Format = TextureFormat::FormatRGB;
		};

		class Texture : public Content::YamlSerializable
		{
		public:
			Texture();
			Texture(const Texture&) = delete;
			~Texture();

			auto GetRendererID() const -> const uint32_t& { return m_RendererID; }

			auto GetWidth() const -> const int32_t& { return m_Width; }
			auto GetHeight() const -> const int32_t& { return m_Height; }
			auto GetFormat() const -> const TextureFormat& { return m_Format; }

			void SetData(const TextureData& data);
			void SetWrapMode(const TextureWrapMode mode) const;
			void SetBorderColor(const Vec4& color) const;
			void SetFilter(const TextureFilter filter) const;
			void AttachToFrameBuffer(const TextureAttachmentSlot slot) const;
			void Bind() const;
			void Bind(const int32_t unit) const;
			void Unbind() const;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			static uint32_t s_CurrentBoundID;

		private:
			uint32_t m_RendererID = -1;

			int32_t m_Width = 0;
			int32_t m_Height = 0;
			TextureFormat m_Format = TextureFormat::FormatRGB;
		};
	}
}
