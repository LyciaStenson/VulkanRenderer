#pragma once

#include <glm/glm.hpp>

namespace VulkanRenderer
{
	struct alignas(16) MeshUBO
	{
		alignas(16)	glm::mat4 model;
	};
}