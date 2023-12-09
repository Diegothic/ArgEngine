#pragma once

#include <arg_pch.hpp>

#include <yaml-cpp/yaml.h>

#include "Core/GUID.hpp"
#include "Core/Math/Math.hpp"

namespace YAML
{
	template<>
	struct convert<Arg::GUID>
	{
		static Node encode(const Arg::GUID& ID)
		{
			Emitter emitter;
			emitter << Hex << ID;

			Node node = Load(emitter.c_str());

			return node;
		}

		static bool decode(const Node& node, Arg::GUID& ID)
		{
			ID = Arg::GUID(node.as<uint64_t>());
			return true;
		}
	};

	template<>
	struct convert<Arg::Vec2>
	{
		static Node encode(const Arg::Vec2& vec)
		{
			Node node;
			node[0] = vec.x;
			node[1] = vec.y;

			return node;
		}

		static bool decode(const Node& node, Arg::Vec2& vec)
		{
			if (!node.IsSequence() || node.size() != 2)
			{
				return false;
			}

			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();

			return true;
		}
	};

	template<>
	struct convert<Arg::Vec3>
	{
		static Node encode(const Arg::Vec3& vec)
		{
			Node node;
			node[0] = vec.x;
			node[1] = vec.y;
			node[2] = vec.z;

			return node;
		}

		static bool decode(const Node& node, Arg::Vec3& vec)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}

			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			vec.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<Arg::Vec4>
	{
		static Node encode(const Arg::Vec4& vec)
		{
			Node node;
			node[0] = vec.x;
			node[1] = vec.y;
			node[2] = vec.z;
			node[3] = vec.w;

			return node;
		}

		static bool decode(const Node& node, Arg::Vec4& vec)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			vec.z = node[2].as<float>();
			vec.w = node[3].as<float>();

			return true;
		}
	};
}
