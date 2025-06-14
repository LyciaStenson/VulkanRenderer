#pragma once

#include <glm/glm.hpp>

namespace VulkanRenderer
{
	struct alignas(16) MaterialFactorsUBO
	{
		alignas(16)	glm::vec4 baseColor;
		alignas(16)	glm::vec3 metallicRoughness;
	};
}