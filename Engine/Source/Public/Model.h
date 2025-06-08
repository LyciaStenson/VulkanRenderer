#pragma once

#include <unordered_map>
#include <string>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

namespace VulkanRenderer
{
	struct Model
	{
		std::string name;
		std::unordered_map<size_t, std::string> meshIndexToName;
		fastgltf::Asset gltfAsset;
		size_t sceneIndex;
	};
}