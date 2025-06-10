#pragma once

#include <glm/glm.hpp>

namespace VulkanRenderer
{
	struct MaterialFactorsUBO
	{
		alignas(16) glm::vec4 baseColor;
		alignas(16) float metallic;
		alignas(16) float roughness;
	};
}