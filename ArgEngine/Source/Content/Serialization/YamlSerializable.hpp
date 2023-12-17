#pragma once

#include <arg_pch.hpp>

#include "Content/Yaml/Yaml.hpp"
#include "ISerializable.hpp"

namespace Arg
{
	namespace Content
	{
		class YamlSerializable : public ISerializable
		{
		public:
			auto Serialize(const std::filesystem::path& destination) const -> bool override final;
			auto Deserialize(const std::filesystem::path& source) -> bool override final;

		public:
			auto Serialize(YAML::Node& node) const -> bool;
			auto Deserialize(const YAML::Node& node) -> bool;

		protected:
			virtual auto VOnSerialize(YAML::Node& node) const -> bool = 0;
			virtual auto VOnDeserialize(const YAML::Node& node) -> bool = 0;

			template <typename TValueType>
			auto ValueOr(
				const YAML::Node& node,
				const TValueType& alternative
			) const->TValueType;
		};

		template <typename TValueType>
		auto YamlSerializable::ValueOr(
			const YAML::Node& node,
			const TValueType& alternative
		) const -> TValueType
		{
			return node
				? node.as<TValueType>()
				: alternative;
		}
	}
}
