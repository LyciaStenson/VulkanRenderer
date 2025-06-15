#pragma once

#include <glm/glm.hpp>

namespace VulkanRenderer
{
	constexpr int MAX_POINT_LIGHTS = 16;

	struct alignas(16) PointLightData
	{
		alignas(16) glm::vec4 positionRadius;
		alignas(16) glm::vec4 colorIntensity;
	};

	struct alignas(16) PointLightsUBO
	{
		alignas(4) uint32_t lightsNum;
		
		alignas(16) PointLightData lighs[MAX_POINT_LIGHTS];
	};
}