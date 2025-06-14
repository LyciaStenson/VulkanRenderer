#pragma once

#include <glm/glm.hpp>

namespace VulkanRenderer
{
	constexpr int MAX_POINT_LIGHTS = 16;

	struct alignas(16) PointLightData
	{
		alignas(16)	glm::vec3 position;
		alignas(16)	glm::vec3 color;
		alignas(4)	float radius;
		alignas(4)	float intensity;
	};

	struct alignas(16) PointLightsUBO
	{
		alignas(4) uint32_t lightsNum;
		
		alignas(16) PointLightData lighs[MAX_POINT_LIGHTS];
	};
}