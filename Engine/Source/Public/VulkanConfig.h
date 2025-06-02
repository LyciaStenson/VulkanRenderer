#pragma once

#include <vector>

namespace VulkanConfig
{
	extern constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	extern bool enableValidationLayers;

	extern const std::vector<const char*> validationLayers;

	extern const std::vector<const char*> deviceExtensions;

	extern bool CheckValidationLayerSupport();

	extern void InitializeVulkanConfig();
}