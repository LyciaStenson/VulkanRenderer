#pragma once

#include <glm/glm.hpp>

namespace VulkanRenderer
{
	struct MeshUBO
	{
		alignas(16) glm::mat4 model;
	};
}